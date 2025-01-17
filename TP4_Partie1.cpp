#include <iostream>
#include <cmath>
#include <tuple>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <utility>
#include <array>
#include <ostream>

//--------------------------------------------------------------------------------------
/*
  Le but de ce TP est de mettre en place un petit systÃ¨me d'EXPRESSION TEMPLATES
  Les EXPRESSION TEMPLATES sont une technique d'optimisation de calcul numÃ©rique qui
  utilisent la mÃ©ta-programmation pour construire une reprÃ©sentation lÃ©gÃ©re d'une formule
  arbitraire sous la forme d'un ARBRE DE SYNTAXE ABSTRAITE.

  Une fois construit Ã  la compilation, cet arbre devient exploitable Ã  la compilation 
  ou Ã  l'exÃ©cution pour effectuer des calculs de divers types. 

  RÃ©pondez au questions dans l'ordre en complÃ©tant le code.
*/
//---------------------------------------------------------------------------------------
namespace et
{
  // Q1 - DÃ©finissez un concept expr qui est valide si un type T fournit un membre T::is_expr()
  template<typename T>
  concept expr = requires( T t )
  {
    { T::is_expr() } -> std::same_as<bool>;  
  };

  //---------------------------------------------------------------------------------------
  /*
    Le premier Ã©lÃ©ment fondamental d'un systeme d'EXPRESSION TEMPLATE est la classe de 
    terminal. Un TERMINAL reprÃ©sente une feuille de l'ARBRE DE SYNTAXE. Dans notre cas,
    nos terminaux sont numÃ©rotÃ© statiquement pour reprÃ©sent diffÃ©rentes variables.

    Q2. ComplÃ©tez l'implÃ©mentation de la structure template terminal ci dessous en suivant les demandes
  */
  //---------------------------------------------------------------------------------------
  template<int ID>
  struct terminal 
  {
    // Faite en sorte que terminal vÃ©rifie le concept expr
    static constexpr bool is_expr() { return true; }


    std::ostream& print(std::ostream& os) const
    {
      // Pour terminal<I>, affiche "arg<I>" et renvoit os.
      os << "arg<" << ID << ">";
      return os; 
    }

    template<typename... Args>
    constexpr auto operator()(Args&&... args) const
    {
      // Construit un tuple de tout les args et renvoit le ID-eme via std::get
      // Veillez Ã  bien repsecter le fait que args est une reference universelle
      return std::get<ID>(std::forward_as_tuple(std::forward<Args>(args)...)); 
    }
  };
    // Generateur de variable numÃ©rotÃ©e
  template<int ID>
  inline constexpr auto arg =  terminal<ID>{};

  // Variables _0, _& et _2 sont prÃ©dÃ©finies
  inline constexpr auto _0  = arg<0>;
  inline constexpr auto _1  = arg<1>;
  inline constexpr auto _2  = arg<2>;
  
    //---------------------------------------------------------------------------------------
  /*
    Le deuxieme Ã©lÃ©ment  d'un systeme d'EXPRESSION TEMPLATE est la classe de noeud. 
    Un NODE reprÃ©sente un opÃ©ratuer ou une fonction dans l'ARBRE DE SYNTAXE. 

    Il est dÃ©finit par le type de l'OPERATION effectuÃ©e au passage du noeud et d'une
    liste variadique de ses sous-nodes.

    Q3 ComplÃ©tez l'implÃ©mentation de la structure template node ci dessous en suivant les demandes
  */
  //---------------------------------------------------------------------------------------
  template<typename Op, typename... Children>
  struct node
  {
    // Faite en sorte que node vÃ©rifie le concept expr
    static constexpr bool is_expr() { return true; }

    // Construisez un node Ã  partir d'une instande de Op et d'une liste variadique de Children
    // Ce constructeur sera constexpr
    constexpr node(Op op, Children... children)
      : op(op), children(std::make_tuple(children...)) {}
 

    // L'operateur() de node permet d'avaluer le sous arbre courant de maniÃ¨re 
    // rÃ©cursive. Les paramÃ¨tres args... reprÃ©sentent dans l'ordre les valeurs des
    // variables contenus dans le sous arbre.
    // Par exemple, le node {op_add, terminal<1>, termnal<0>} appelant operator()(4, 9)
    // doit renvoyer op_add(9, 4);
    // Renseignez vous sur std::apply pour vous simplifier la vie
    // Pensez qu'un node contient potentiellement d'autre node.
    template<typename... Args>
    constexpr auto operator()(Args&&... args) const
    {
      return std::apply(
        [&](const auto&... children) {
          return op(children(args...)...);
        },
        children
      );
    }

    // Affiche un node en demandant Ã  Op d'afficher les sous arbres
    std::ostream& print(std::ostream& os) const
    {
      auto arr = std::apply(
        [&](auto const&... child){
          return std::array<std::string, sizeof...(Children)>{
            to_string(child)...
          };
        },
        children
      );

      // On décompose le tableau pour que op.print recoive chaque chaîne séparément
      return std::apply(
        [&](auto const&... s) -> std::ostream& {
          return op.print(os, s...);
        },
        arr
      );
    }

    // Helper qui convertit un enfant (terminal ou node) en string
    static std::string to_string(auto const& sub)
    {
    std::ostringstream s;
    sub.print(s);
    return s.str();
    }

    // Op est stockÃ©e par valeur
    // les Children... sont stockÃ©es dans un tuple
    Op  op;
    std::tuple<Children...>  children;
  };

  //----------------------------------------------
  /*
    add_ est un exemple de type d'operation passable Ã  un node
    Il fournit un operator() qui effectue le calcul et une fonction
    print qui affiche le node.
  */
  //----------------------------------------------
  struct add_ 
  {
    constexpr auto operator()(auto a, auto b) const
    {
      return a + b;
    }

    std::ostream& print(std::ostream& os, auto a, auto b) const
    {
      return os << a << " + " << b ;
    }
  };
  
  // On lui associe un operator+ qui consomme des expr et renvoit le node
  template<expr L, expr R>
  constexpr auto operator+(L l, R r)
  {
    return node{add_{}, l, r};
  }

  /*
    Q4. Sur le modÃ¨le de add_, implÃ©mentez 
      - mul_ et un operator* pour la multiplication
      - abs_ et une fonction abs pour le calcul de la valeur absolue
      - fma_ et une fonction fma(a,b,c) qui calcul a*b+c
  */
struct mul_ 
  {
    constexpr auto operator()(auto a, auto b) const
    {
      return a * b;
    }

    std::ostream& print(std::ostream& os, auto a, auto b) const
    {
      return os << a << " * " << b ;
    }
  };

  template<expr L, expr R>
  constexpr auto operator*(L l, R r)
  {
    return node{mul_{}, l, r};
  }

  struct abs_
  {
    constexpr auto operator()(auto a) const
    {
      return std::abs(a);
    }

    std::ostream& print(std::ostream& os, auto a) const
    {
      return os << "|" << a << "|";
    }
  };

  template<expr L>
  constexpr auto abs(L l)
  {
    return node{abs_{}, l};
  }

  struct fma_
  {
    constexpr auto operator()(auto a, auto b, auto c) const
    {
      return a * b + c;
    }

    std::ostream& print(std::ostream& os, auto a, auto b, auto c) const
    {
      return os << a << " * " << b << " + " << c;
    }
  };

  template<expr A, expr B, expr C>
  constexpr auto fma(A a, B b, C c)
  {
    return node{fma_{}, a, b, c};
  }

}  
int main() 
{
    
    // TESTS SUJETS 
    
    constexpr auto f = et::fma(et::_1, abs(et::_2),et::_0);
    f.print(std::cout) << "\n";
    std::cout << f(1,2,3) << "\n";
    
    // Testing fma operation
    std::cout << "#### fma operation testing : " << "\n";
    
    // Test fma with absolute value
    constexpr auto expr_fma_1 = et::fma(et::_1, abs(et::_2), et::_0);
    expr_fma_1.print(std::cout) << "\n";
    
    std::cout << "Testing fma(1, 2, 3) = " << expr_fma_1(1, 2, 3) << "\n";
    assert(expr_fma_1(1, 2, 3) == 7); // Expected: 7
    
    std::cout << "Testing fma(1, 2, -3) = " << expr_fma_1(1, 2, -3) << "\n";
    assert(expr_fma_1(1, 2, -3) == 7); // Expected: 7

    // Test fma without absolute value
    constexpr auto expr_fma_2 = et::fma(et::_1, et::_2, et::_1);
    expr_fma_2.print(std::cout) << "\n";
    
    std::cout << "Testing fma(1, 2, 3) = " << expr_fma_2(1, 2, 3) << "\n";
    assert(expr_fma_2(1, 2, 3) == 8); // Expected: 8
    
    std::cout << "Testing fma(1, 2, -3) = " << expr_fma_2(1, 2, -3) << "\n";
    assert(expr_fma_2(1, 2, -3) == -4); // Expected: -4

    std::cout << "\n" << "#### addition testing : " << "\n";
    
    // Test addition expression
    constexpr auto sum_expr = et::_0 + et::_1;
    sum_expr.print(std::cout) << "\n";
    
    std::cout << "Testing sum(3, 6) = " << sum_expr(3, 6) << "\n";
    assert(sum_expr(3, 6) == 9); // Expected: 9
    
    std::cout << "Testing sum(-4, 3) = " << sum_expr(-4, 3) << "\n";
    assert(sum_expr(-4, 3) == -1); // Expected: -1
    
    std::cout << "Testing sum(-4.2, 5.7) = " << sum_expr(-4.2, 5.7) << "\n";
    assert(std::abs(sum_expr(-4.2, 5.7) - 1.5) < 1e-12); // Expected: 1.5
    
    std::cout << "Testing sum(-4.2, 3) = " << sum_expr(-4.2, 3) << "\n";
    assert(std::abs(sum_expr(-4.2, 3) + 1.2) < 1e-12); // Expected: 1.2

    std::cout << "\n" << "#### multiplication testing : " << "\n";
    
    // Test multiplication expression
    constexpr auto product_expr = et::_0 * et::_1;
    product_expr.print(std::cout) << "\n";
    
    std::cout << "Testing product(3, 6) = " << product_expr(3, 6) << "\n";
    assert(product_expr(3, 6) == 18); // Expected: 18
    
    std::cout << "Testing product(-4, 7) = " << product_expr(-4, 7) << "\n";
    assert(product_expr(-4, 7) == -28); // Expected: -28
    
    std::cout << "Testing product(3.4, 6.5) = " << product_expr(3.4, 6.5) << "\n";
    assert(std::abs(product_expr(3.4, 6.5) - 22.1) < 1e-12); // Expected: 22.1
    
    std::cout << "Testing product(-4, 7.2) = " << product_expr(-4, 7.2) << "\n";
    assert(std::abs(product_expr(-4, 7.2) + 28.8) < 1e-12); // Expected: 28.8

    std::cout << "\n" << "#### combination of operations : " << "\n";
    
    // Combining operations: fma and multiplication
    constexpr auto combined_expr = et::fma(et::_0, et::_1, et::_2);
    combined_expr.print(std::cout) << "\n";
    product_expr.print(std::cout) << "\n";
    
    std::cout << "Evaluating combined expression fma(1, 2, product(3, 5)) = " 
              << combined_expr(1, 2, product_expr(3, 5)) << "\n";
    assert(combined_expr(1, 2, product_expr(3, 5)) == 17); // Expected: 17

    return 0;
}


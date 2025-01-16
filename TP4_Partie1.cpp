#include <iostream>
#include <cmath>
#include <tuple>
#include <algorithm>

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
      os << "(";
      std::apply(
        [&](const auto&... children) {
          ((children.print(os), os << ","), ...);
        },
        children
      );
      os << ")";
      return os; 
    }
    
    // Op est stockÃ©e par valeur
    // les Children... sont stockÃ©es dans un tuple
    Op  op;
    std::tuple<Children...>  children;
  };


}  
int main()
{
    using namespace et;
    
    //*********************************** TESTS TERMINAL *****************************************//
    // Test de la structure terminal
    terminal<0> t0;
    terminal<1> t1;
    terminal<2> t2;

    // Affichage des terminaux
    std::cout << "Affichage des terminaux:" << std::endl;
    t0.print(std::cout) << std::endl;
    t1.print(std::cout) << std::endl;
    t2.print(std::cout) << std::endl;

    // Test de l'opérateur ()
    std::cout << "\nTests de l'opérateur () :" << std::endl;
    auto value0 = t0(42, 3.14, "Hello");
    std::cout << "Value0: " << value0 << std::endl; // Devrait afficher 42

    auto value1 = t1(42, 3.14, "Hello");
    std::cout << "Value1: " << value1 << std::endl; // Devrait afficher 3.14

    auto value2 = t2(42, 3.14, "Hello");
    std::cout << "Value2: " << value2 << std::endl; // Devrait afficher "Hello"

    // Test avec les variables prédéfinies
    std::cout << "\nTests avec les variables prédéfinies :" << std::endl;
    std::cout << "_0: " << _0(1, 2, 3) << std::endl; // Devrait afficher 1
    std::cout << "_1: " << _1(1, 2, 3) << std::endl; // Devrait afficher 2
    std::cout << "_2: " << _2(1, 2, 3) << std::endl; // Devrait afficher 3

    // Tests supplémentaires
    std::cout << "\nTests supplémentaires :" << std::endl;

    // Test avec des types différents
    auto mixedValue = _1("First", 3.14159, 42);
    std::cout << "MixedValue (_1): " << mixedValue << std::endl; // Devrait afficher 3.14159

    // Test avec une seule variable
    auto singleValue = _0(99);
    std::cout << "SingleValue (_0): " << singleValue << std::endl; // Devrait afficher 99

    // Test hors limites (compile-time error attendue si on active cette ligne)
    // auto outOfBounds = _3(1, 2, 3); // _3 n'existe pas

    // Test avec un nombre élevé d'arguments
    //auto manyArgsValue = _3(10, 20, 30, 40, 50, 60);
    //std::cout << "ManyArgsValue (_3): " << manyArgsValue << std::endl; // Devrait afficher 40

    // Test avec une fonction lambda pour valider l'usage avancé
    auto lambdaValue = _2([] { return "Lambda"; }, "Second", 3.14);
    std::cout << "LambdaValue (_2): " << lambdaValue << std::endl; // Devrait afficher 3.14
    
    //*********************************** TESTS NODE *****************************************//
    
    // Opérateur d'addition pour les tests
    auto op_add = [](auto a, auto b) { return a + b; };

    // Construction de noeuds simples
    node add_node(op_add, _0, _1);

    // Test d'évaluation
    std::cout << "Evaluation: " << add_node(3, 7) << std::endl; // Devrait afficher 10

    // Test d'affichage
    std::cout << "Affichage du node: ";
    add_node.print(std::cout);
    std::cout << std::endl;

    // Tests imbriqués
    node nested_node(op_add, add_node, _2);
    std::cout << "Evaluation imbriquée: " << nested_node(1, 2, 3) << std::endl; // Devrait afficher 6 (1+2+3)

    std::cout << "Affichage du node imbriqué: ";
    nested_node.print(std::cout);
    std::cout << std::endl;
    
    return 0;
}

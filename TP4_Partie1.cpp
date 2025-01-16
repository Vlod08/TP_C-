int main()
{
    using namespace et;

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
    auto manyArgsValue = _3(10, 20, 30, 40, 50, 60);
    std::cout << "ManyArgsValue (_3): " << manyArgsValue << std::endl; // Devrait afficher 40

    // Test avec une fonction lambda pour valider l'usage avancé
    auto lambdaValue = _2([] { return "Lambda"; }, "Second", 3.14);
    std::cout << "LambdaValue (_2): " << lambdaValue << std::endl; // Devrait afficher 3.14

    return 0;
}

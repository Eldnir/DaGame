#include <iostream>
#include <stdio.h>
#include <conio.h> //pour les input
#include <cstdlib>
#include <windows.h>
#include <string>
#include <ctime>
#include <vector>
using namespace std;

//tous les trucs avec hconsole genre  SetConsoleTextAttribute(hConsole, 11); c'est pour gerer la couleur du texte
//j'ai rendu l'attaque de base  du mage cheaté pour pouvoir avancer plus vite pour test les trucs

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

struct Sort
{
    string nom;
    int Degats;
    int Cout;
};

typedef vector<Sort> liste_de_sorts;

bool GameOver(false);

struct Player
{
    string Classe;
    int Niveau;
    liste_de_sorts Sorts;
    int Mana;
    int Mana_max;
    int HP;
    int HP_max;
    int Potions;
    int Gold;
    int Degats;
    int Defense;
    //ajouter des stats si besoin
};

Player joueur;

Sort attaque_basique_guerrier{"coup d'epee", 12, 0};
Sort attaque_basique_mage{"boule de feu basique", 100, 0}; // MODIFIER LES DEGATS, J'AI MIS 100 POUR TEST PLUS VITE
Sort attaque_basique_archer{"tirer une fleche", 10, 0};
Sort boule_de_feu{"tornade de feu", 20, 5};
Sort fleche_perforante{"fleche_perforante", 15, 1};
Sort charge{"charge", 10, 1};
Sort attaque_basique_ennemi{"boum", 4, 0};
Sort attaque_speciale_gobelin{"fourberie", 6, 0};
Sort attaque_speciale_voleur{"lancer de couteau", 8, 0};
Sort attaque_speciale_geant{"TAPER", 5, 0};

struct Ennemi
{
    string Nom;
    int Niveau;
    liste_de_sorts Sorts;
    int HP;
    int HP_max;
    int Gold_weight;
    int Degats;
    int Defense;
};

class Item
{
public:
    string Nom;
    string Effet;
    int Cout;
    int Color; //permet de l'afficher de la bonne couleur.  2 pour vert(commun), 11 (rare), 13 (epique)

    //void Application_effet(Player &joueur);
};

//tous les items du jeu
Item potion_de_soin{"Potion de soin", "+1 potion", 4, 15};
Item upgrade_basic_attack{"Amelioration de la competence de base", "+5", 5, 2};
Item upgrade_special_attack{"Amelioration de la competence speciale", "+5", 6, 2};
Item epee_longue{"Epee longue", "+4 degats", 8, 11};
Item death_dance{"Death Dance", "+20 degats", 20, 13};
vector<Item> objets_shop{potion_de_soin, upgrade_basic_attack, upgrade_special_attack, epee_longue, death_dance};

//prototypes
void init();
void tour_de_jeu();
void shop(Player joueur);
void affichage_combat(Player joueur, Ennemi adversaire, int tours_consecutifs, int tour);
void affichage_arrivee_ennemi(Ennemi adversaire);
Ennemi create_adversaire(Player joueur);
int manage_degats_joueur(Player joueur, Ennemi adversaire, Sort sort_utilise);
int manage_degats_ennemi(Ennemi adversaire, Player joueur, Sort sort_utilise);

int main()
{
    init();
    tour_de_jeu();
    return 0;
}

void init()
{
    system("CLS");
    SetConsoleTextAttribute(hConsole, 11);
    cout << "Bienvenue, aventurier. Vous allez arriver dans le fantastique monde de Rominouland" << endl;
    cout << "Choisissez votre classe (ecrivez en toute lettres)" << endl;
    SetConsoleTextAttribute(hConsole, 12);
    cout << "Guerrier, ";
    SetConsoleTextAttribute(hConsole, 5);
    cout << "Mage, ";
    SetConsoleTextAttribute(hConsole, 2);
    cout << "Archer" << endl;
    SetConsoleTextAttribute(hConsole, 15);
demander_classe:
    cin >> joueur.Classe;
    joueur.Niveau = 1;
    if (joueur.Classe == "Guerrier")
    {
        joueur.Sorts = {attaque_basique_guerrier, charge};
        joueur.Mana_max = 1;
        joueur.Mana = joueur.Mana_max;
        joueur.HP_max = 30;
        joueur.HP = joueur.HP_max;
        joueur.Potions = 2;
        joueur.Gold = 0;
        joueur.Degats = 1;
        joueur.Defense = 5;
    }
    else if (joueur.Classe == "Mage")
    {
        joueur.Sorts = {attaque_basique_mage, boule_de_feu};
        joueur.Mana_max = 30;
        joueur.Mana = joueur.Mana_max;
        joueur.HP_max = 20;
        joueur.HP = joueur.HP_max;
        joueur.Potions = 2;
        joueur.Gold = 0;
        joueur.Degats = 2;
        joueur.Defense = 2;
    }
    else if (joueur.Classe == "Archer")
    {
        joueur.Sorts = {attaque_basique_archer, fleche_perforante};
        joueur.Mana_max = 4;
        joueur.Mana = joueur.Mana_max;
        joueur.HP_max = 25;
        joueur.HP = joueur.HP_max;
        joueur.Potions = 2;
        joueur.Gold = 0;
        joueur.Degats = 2;
        joueur.Defense = 3;
    }
    else
    {
        cout << "veuillez entrer une reponse adequate" << endl; //pour recommencer si pas la bonne syntaxe
        goto demander_classe;
    }
    SetConsoleTextAttribute(hConsole, 11);
    cout << "tres bien, vous etes donc un ";
    SetConsoleTextAttribute(hConsole, 5);
    cout << joueur.Classe;
    SetConsoleTextAttribute(hConsole, 11);
    cout << " de niveau " << joueur.Niveau << endl
         << endl
         << endl;
    cout << "tous les 3 paliers, vous aurez la possibilite d'aller au magasin." << endl;
    Sleep(2000);
    cout << "Si vous choisissez de ne pas y aller, vous gagnerez plus de pieces, moyennant le risque de mourir en chemin." << endl;
    Sleep(2000);
    cout << "je dois y aller. Prenez garde, aventurier !" << endl;
    Sleep(2000);
}

void tour_de_jeu()
{
    int tours_consecutifs(1);
    int nb_recommencer(1);

nouveau_combat:
    int tour(1);
    int action(0);
    Ennemi adversaire;

    adversaire = create_adversaire(joueur);

    affichage_arrivee_ennemi(adversaire);

debut_tour:
    affichage_combat(joueur, adversaire, tours_consecutifs, tour);

choix_action:
    cout << "que faire ?" << endl;
    cout << "1 - " << joueur.Sorts[0].nom << endl;
    cout << "2 - " << joueur.Sorts[1].nom << endl;

    if (joueur.HP < joueur.HP_max && joueur.Potions > 0)
    {
        cout << "3 - utiliser une potion" << endl;
    }

    cin >> action;
    if (action == 1)
    {
        cout << "joli coup ! l'adversaire perd " << manage_degats_joueur(joueur, adversaire, joueur.Sorts[0]) << " HP" << endl;
        adversaire.HP -= manage_degats_joueur(joueur, adversaire, joueur.Sorts[0]);
    }
    else if (action == 2)
    {
        if ((joueur.Mana - joueur.Sorts[1].Cout) >= 0)
        {
            cout << "vous lancez " << joueur.Sorts[1].nom << " , et infligez " << manage_degats_joueur(joueur, adversaire, joueur.Sorts[1]) << " points de degats a l'ennemi" << endl;
            adversaire.HP -= manage_degats_joueur(joueur, adversaire, joueur.Sorts[1]);
            joueur.Mana -= joueur.Sorts[1].Cout;
        }
        else
        {
            cout << "pas assez de mana restant !" << endl;
            goto choix_action;
        }
    }
    else if (action == 3)
    {
        cout << "vous utilisez une potion !" << endl;
        cout << "vous regagnez 15 HP" << endl;
        joueur.HP += 15;
        joueur.Potions -= 1;
        if (joueur.HP > joueur.HP_max)
        {
            joueur.HP = joueur.HP_max; //pour pas overstack la vie
        }
    }

    Sleep(1000);

    if (adversaire.HP <= 0)
    {
        goto adversaire_mort;
    }
    else
    {
        srand((unsigned int)time(0));
        int ennemi_action = rand() % 4 + 1;
        // pour l'instant, deux valeurs pour hit un coup simple, et une valeur de miss
        cout << "le " << adversaire.Nom << " tente une attaque ..." << endl;
        Sleep(1000);

        if (ennemi_action == 1 || ennemi_action == 2)
        {
            SetConsoleTextAttribute(hConsole, 12);
            cout << "Attaque reussie ! Il vous attaque et vous inflige " << manage_degats_ennemi(adversaire, joueur, adversaire.Sorts[0]) << " points de degats !" << endl;
            joueur.HP -= manage_degats_ennemi(adversaire, joueur, adversaire.Sorts[0]);
            SetConsoleTextAttribute(hConsole, 15);
        }
        else if (ennemi_action == 3)
        {
            SetConsoleTextAttribute(hConsole, 12);
            cout << "Succes critique ! Il utilise la competence " << adversaire.Sorts[1].nom << " et vous inflige " << manage_degats_ennemi(adversaire, joueur, adversaire.Sorts[1]) << " points de degats !" << endl;
            joueur.HP -= manage_degats_ennemi(adversaire, joueur, adversaire.Sorts[1]);
            SetConsoleTextAttribute(hConsole, 15);
        }
        else if (ennemi_action == 4)
        {
            SetConsoleTextAttribute(hConsole, 2);
            cout << "rate !" << endl;
            SetConsoleTextAttribute(hConsole, 15);
        }
    }

    Sleep(4000);

    if (joueur.HP > 0 && adversaire.HP > 0)
    {
        tour += 1;
        system("CLS");
        goto debut_tour;
    }
    else if (adversaire.HP <= 0)
    {
    adversaire_mort:
        SetConsoleTextAttribute(hConsole, 10);
        cout << "Victoire !" << endl;
        SetConsoleTextAttribute(hConsole, 14);
        cout << "vous gagnez " << adversaire.Gold_weight * nb_recommencer << " pieces d'or !" << endl
             << endl
             << endl;
        joueur.Gold += adversaire.Gold_weight * nb_recommencer;
        SetConsoleTextAttribute(hConsole, 15);
        if (tours_consecutifs == 3)
        {
            SetConsoleTextAttribute(hConsole, 3);
            cout << "1 - Aller au magasin ?" << endl;
            SetConsoleTextAttribute(hConsole, 14);
            cout << "2 - Continuer (gold x" << nb_recommencer + 1 << ")" << endl;
            tours_consecutifs = 1;
            int a(0);
            cin >> a;
            if (a == 2) // donc continuer
            {
                joueur.Niveau += 1;
                nb_recommencer += 1;
                Sleep(2000);
                system("CLS");
                goto nouveau_combat;
            }
            else
            {
                shop(joueur);
                system("CLS");
                goto nouveau_combat;
            }
        }
        else
        {
            Sleep(2000);
            tours_consecutifs += 1;
            system("CLS");
            goto nouveau_combat;
        }
    }
    else if (joueur.HP <= 0)
    {
        cout << "Defaite !" << endl;
        return;
    }
}

void shop(Player joueur)
{
    system("CLS");
    SetConsoleTextAttribute(hConsole, 11);
    cout << "Bienvenue, aventurier." << endl;
    Sleep(1000);
debut_shop:
    SetConsoleTextAttribute(hConsole, 11);
    cout << "que souhaitez-vous acheter ?" << endl;
    cout << endl;
    SetConsoleTextAttribute(hConsole, 14);
    cout << "                                                   po : " << joueur.Gold << endl;

    for (size_t i(0); i < objets_shop.size(); i++)
    {
        SetConsoleTextAttribute(hConsole, 15);

        cout << i + 1 << " : ";
        SetConsoleTextAttribute(hConsole, objets_shop[i].Color);
        cout << objets_shop[i].Nom;
        SetConsoleTextAttribute(hConsole, 15);
        cout << " (effet : " << objets_shop[i].Effet << ") ";
        SetConsoleTextAttribute(hConsole, 14);
        cout << "cout : " << objets_shop[i].Cout << " po" << endl;
    }
    cout << objets_shop.size() + 1 << " : rien" << endl;
    //ajouter systeme de lock item au shop
    //ajouer items
    int achat(0);
    cin >> achat;

    if (achat == objets_shop.size() + 1)
    {
        SetConsoleTextAttribute(hConsole, 11);
        cout << "Au revoir" << endl;
        Sleep(2000);
        return;
    }
    if (joueur.Gold - objets_shop[achat - 1].Cout >= 0)
    {
        joueur.Gold-=objets_shop[achat-1].Cout;
        cout << "*application des effets*" << endl;

        /*
        objets_shop[achat - 1].Application_effet(joueur);
        */
    }

    else
    {
        SetConsoleTextAttribute(hConsole, 11);
        cout << "pas assez de pièces d'or" << endl;
        Sleep(1000);
        goto debut_shop;
    }

    SetConsoleTextAttribute(hConsole, 11);
    cout << "autre chose ?" << endl
         << endl;
    SetConsoleTextAttribute(hConsole, 15);
    cout << "1 - Oui" << endl;
    cout << "2 - Non" << endl;

    int autre_chose(0);
    cin >> autre_chose;
    if (autre_chose == 1)
    {
        goto debut_shop;
    }
    else
    {
        SetConsoleTextAttribute(hConsole, 11);
        cout << "Au revoir" << endl;
        Sleep(2000);
        return;
    }
}

void affichage_combat(Player joueur, Ennemi adversaire, int tours_consecutifs, int tour)

{
    SetConsoleTextAttribute(hConsole, 3);
    cout << "----------------------";
    SetConsoleTextAttribute(hConsole, 1);
    cout << "Combat ";
    cout << tours_consecutifs << ", ";

    cout << "Tour" << tour;
    SetConsoleTextAttribute(hConsole, 3);
    cout << "----------------------" << endl;
    cout << "vos stats :  Niveau  : " << joueur.Niveau << endl;
    SetConsoleTextAttribute(hConsole, 12);
    cout << "             HP : " << joueur.HP << "/" << joueur.HP_max << endl;
    SetConsoleTextAttribute(hConsole, 1);
    cout << "             Mana : " << joueur.Mana << "/" << joueur.Mana_max << endl;
    SetConsoleTextAttribute(hConsole, 12);
    cout << "             Potions restantes : " << joueur.Potions << endl;
    SetConsoleTextAttribute(hConsole, 14);
    cout << "             Po : " << joueur.Gold << endl;
    SetConsoleTextAttribute(hConsole, 15);
    cout << "ennemi : " << adversaire.Nom << endl;
    cout << "             Niveau :  " << adversaire.Niveau << endl;
    SetConsoleTextAttribute(hConsole, 12);
    cout << "             HP : " << adversaire.HP << endl;
    SetConsoleTextAttribute(hConsole, 15);
    cout << endl
         << endl;
    return;
}

void affichage_arrivee_ennemi(Ennemi adversaire)
{
    Sleep(1000);
    SetConsoleTextAttribute(hConsole, 12);
    cout << "un ennemi approche !   ...." << endl;
    Sleep(1000);
    SetConsoleTextAttribute(hConsole, 15);
    cout << "c'est un ";
    SetConsoleTextAttribute(hConsole, 5);
    cout << adversaire.Nom;
    SetConsoleTextAttribute(hConsole, 15);
    cout << " de niveau ";
    SetConsoleTextAttribute(hConsole, 12);
    cout << adversaire.Niveau;
    SetConsoleTextAttribute(hConsole, 15);
    cout << " ! " << endl
         << endl;
    Sleep(1000);
}

Ennemi create_adversaire(Player joueur)
{
    Ennemi adversaire;
    srand((unsigned int)time(0));
    int type_ennemi = rand() % 3 + 1;
    if (type_ennemi == 1)
    {
        adversaire.Niveau = joueur.Niveau; // a améliorer pour ajouter un peu de random à ça
        adversaire.Nom = "Gobelin";
        adversaire.Sorts = {attaque_basique_ennemi, attaque_speciale_gobelin};
        adversaire.HP = 20;
        adversaire.Gold_weight = 2;
        adversaire.Defense = 2;
        adversaire.Degats = 3;
        return adversaire;
    }

    else if (type_ennemi == 2)
    {
        adversaire.Niveau = joueur.Niveau; // a améliorer pour ajouter un peu de random à ça
        adversaire.Nom = "Voleur";
        adversaire.Sorts = {attaque_basique_ennemi, attaque_speciale_voleur};
        adversaire.HP = 30;
        adversaire.Gold_weight = 3;
        adversaire.Defense = 3;
        adversaire.Degats = 4;

        return adversaire;
    }
    else if (type_ennemi == 3)
    {
        adversaire.Niveau = joueur.Niveau; // a améliorer pour ajouter un peu de random à ça
        adversaire.Nom = "Geant";
        adversaire.Sorts = {attaque_basique_ennemi, attaque_speciale_geant};
        adversaire.HP = 40;
        adversaire.Gold_weight = 4;
        adversaire.Defense = 3;
        adversaire.Degats = 2;
        return adversaire;
    }
}

int manage_degats_joueur(Player joueur, Ennemi adversaire, Sort sort_utilise)
{
    /* 
juste une idée comme ça, a voir si c'est equilibré
pour l'instant, ce que ca fait c'est que les degats sont calculés de la facon suivante :
degats aléatoires du sort entre 1 et la stat de degats du sort
a ça on ajoute 2* le niveau du joueur
a ça on ajoute les degats du joueur
et on soustrait par la défense de l'adversaire/2
*/
    srand((unsigned int)time(0));
    int degats = rand() % sort_utilise.Degats + 1; // changer pour que ça fasse un nombre aléatoire entre degats min sort et degat max sort peut etre ?
    degats = degats + (2 * joueur.Niveau);
    degats = degats + (joueur.Degats);
    degats = degats - (adversaire.Defense / 2);
    return degats;
}
int manage_degats_ennemi(Ennemi adversaire, Player joueur, Sort sort_utilise)

{
    srand((unsigned int)time(0));
    int degats = rand() % sort_utilise.Degats + 1; // changer pour que ça fasse un nombre aléatoire entre degats min sort et degat max sort peut etre ?
    degats = degats + (2 * adversaire.Niveau);
    degats = degats + (adversaire.Degats);
    degats = degats - (joueur.Defense / 2);
    return degats;
}

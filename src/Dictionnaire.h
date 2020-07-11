/**
 * \file Dictionnaire.h
 * \brief Ce fichier contient l'interface d'un dictionnaire.
 * \author IFT-2008, Guillaume Doucet
 * \version 0.1
 * \date juillet 2020
 *
 */


#ifndef DICO_H_
#define DICO_H_

#include <iostream>
#include <fstream> // pour les fichiers
#include <string>
#include <vector>
#include <queue>


/**
 * \namespace TP3
 * \brief Espace de nom qui contient les classes du tp3.
 */
namespace TP3
{

/**
 * \class Dictionnaire
 * \brief classe représentant un dictionnaire de traductions sous forme d'un arbre AVL
 *
 *  Attributs:
 *      - NoeudDictionnaire * racine, racine de l'arbre AVL
        - int cpt, nombre de mot
 *
 */

class Dictionnaire
{
public:

	//Constructeur
	Dictionnaire();

	//Constructeur de dictionnaire à partir d'un fichier
	//Le fichier doit être ouvert au préalable
	explicit Dictionnaire(std::ifstream &fichier);

	//Destructeur.
	~Dictionnaire();

	//Ajouter un mot au dictionnaire et l'une de ses traductions en équilibrant l'arbre AVL
	void ajouteMot(const std ::string& motOriginal, const std ::string& motTraduit);

	//Supprimer un mot et équilibrer l'arbre AVL
	//Si le mot appartient au dictionnaire, on l'enlève et on équilibre. Sinon, on ne fait rien.
	//Exception	logic_error si l'arbre est vide
	//Exception	logic_error si le mot n'appartient pas au dictionnaire
	void supprimeMot(const std ::string& motOriginal);

	//Quantifier la similitude entre 2 mots (dans le dictionnaire ou pas)
	//Ici, 1 représente le fait que les 2 mots sont identiques, 0 représente le fait que les 2 mots sont complètements différents
	//On retourne une valeur entre 0 et 1 quantifiant la similarité entre les 2 mots donnés
	//Vous pouvez utiliser par exemple la distance de Levenshtein, mais ce n'est pas obligatoire !
	double similitude(const std ::string& mot1, const std ::string& mot2);


	//Suggère des corrections pour le mot motMalEcrit sous forme d'une liste de mots, dans un vector, à partir du dictionnaire
	//S'il y a suffisament de mots, on redonne 5 corrections possibles au mot donné. Sinon, on en donne le plus possible
	//Exception	logic_error si le dictionnaire est vide
	std::vector<std::string> suggereCorrections(const std ::string& motMalEcrit);

	//Trouver les traductions possibles d'un mot
	//Si le mot appartient au dictionnaire, on retourne le vecteur des traductions du mot donné.
	//Sinon, on retourne un vecteur vide
	std::vector<std::string> traduit(const std ::string& mot);

	//Vérifier si le mot donné appartient au dictionnaire
	//On retourne true si le mot est dans le dictionnaire. Sinon, on retourne false.
	bool appartient(const std::string &mot);

	//Vérifier si le dictionnaire est vide
	bool estVide() const;

	//Affiche à l'écran l'arbre niveau par niveau de façon à voir si l'arbre est bien balancé.
	//Ne touchez pas s.v.p. à cette méthode !
    friend std::ostream& operator<<(std::ostream& out, const Dictionnaire& d)
    {
  	  if (d.racine != 0)
  	  {
  		  std::queue<NoeudDictionnaire*> file;
  		  std::queue<std::string> fileNiveau;

  		  NoeudDictionnaire * noeudDicotemp;
  		  std::string niveauTemp;

  		  int hg = 0, hd = 0;

  		  file.push(d.racine);
  		  fileNiveau.push("1");
  		  while (!file.empty())
  		  {
  			  noeudDicotemp = file.front();
  			  niveauTemp = fileNiveau.front();
  			  out << noeudDicotemp->mot;
  			  if (noeudDicotemp->gauche == 0) hg = -1; else hg = noeudDicotemp->gauche->hauteur;
  			  if (noeudDicotemp->droite == 0) hd = -1; else hd = noeudDicotemp->droite->hauteur;
  			  out << ", " << hg - hd;
  			  out << ", " << niveauTemp;
  			  out << std::endl;
  			  file.pop();
  			  fileNiveau.pop();
  			  if (noeudDicotemp->gauche != 0)
  			  {
  				  file.push(noeudDicotemp->gauche);
  				  fileNiveau.push(niveauTemp + ".1");
  			  }
  			  if (noeudDicotemp->droite != 0)
  			  {
  				  file.push(noeudDicotemp->droite);
  				  fileNiveau.push(niveauTemp + ".2");
  			  }
  		  }
  	  }
  	  return out;
    }


private:

    /**
     * \class NoeudDictionnaire
     * \brief Classe interne représentant un noeud dans l'arbre AVL constituant le dictionnaire de traduction.
     *
     *  Propriétés (accessible par dictionnaire):
     * 	- mot : le mot contenus dans le noeud
     * 	- traductions : les traductions possibles du mot
     * 	- gauche : pointeur vers le noeud enfant gauche
     * 	- droite : pointeur vers le noeud enfant droite
     * 	- hauteur : hauteur du noeud dans l'arbre
     *
     */
	class NoeudDictionnaire
	{
	public:

		std::string mot;						// Un mot (en anglais)

		std::vector<std::string> traductions;	// Les différentes traductions possibles en français du mot en anglais
												// Par exemple, la liste française { "contempler", "envisager" et "prévoir" }
												// pourrait servir de traduction du mot anglais "contemplate".

	    NoeudDictionnaire *gauche, *droite;		// Les enfants du noeud

	    int hauteur;							// La hauteur de ce noeud (afin de maintenir l'équilibre de l'arbre AVL)

        explicit NoeudDictionnaire(const std::string& p_mot, const std::string& traduction) :
                mot(p_mot), gauche(nullptr), droite(nullptr), hauteur(0), traductions() {
            traductions.push_back(traduction);
        }
	};

    /**
     * \var racine
     * \brief La racine de l'arbre des mots contenus dans une instance de la classe privée NoeudDictionnaire
     */
	NoeudDictionnaire * racine;
    /**
     * \var cpt
     * \brief Le nombre de mots dans le dictionnaire
     */
    int cpt;
    const unsigned int SIMILITUDE_MAX = 1; //plus grande valeur de similutude possible
    const double SIMILITUDE_MIN_POUR_SUGGESTION = 0.2; //similitude minimale acceptable


    //region private methods

    void _detruireRecursif(NoeudDictionnaire*& arbre);
    void _ajouterRecursif(NoeudDictionnaire*& node, const std::string &motOriginal, const std::string &motTraduit);
    NoeudDictionnaire* _trouverRecursif(NoeudDictionnaire* const & node, const std::string &motAtrouver, double similitudeMinimum);
    void _supprimerMotRecursif(NoeudDictionnaire*& node, const std::string &motAenlever);
    NoeudDictionnaire* _noeudMinimalRecusrif(NoeudDictionnaire* const & node);

    bool _vecteurContient(const std::vector<std::string> &vecteur, const std::string& element);
    void _updateHauteurNoeud(NoeudDictionnaire*& node);
    int _hauteur(NoeudDictionnaire*& node);
    void _balancerUnNoeud(NoeudDictionnaire*& node);
    bool _baseEstPlustPetitQue(const std::string& base, const std::string& compare);
    bool _baseEstPlustGrandQue(const std::string& base, const std::string& compare);
    bool _possedeEnfantUnique(NoeudDictionnaire* const & node);
    void _swapNodes(NoeudDictionnaire*& nodeBase, NoeudDictionnaire*& nodeToSwapTo);
    unsigned int _distanceLevenshtein(const std::string &mot1, const std::string &mot2);

    bool _debalancementAGauche(NoeudDictionnaire*& node);
    bool _debalancementADroite(NoeudDictionnaire*& node);
    bool _desequilibreSecondaireAGauche(NoeudDictionnaire*& node);
    bool _desequilibreSecondaireADroite(NoeudDictionnaire*& node);

    void _zigZagGauche(NoeudDictionnaire*& noeudCritique);
    void _zigZigGauche(NoeudDictionnaire*& noeudCritique);
    void _zigZagDroite(NoeudDictionnaire*& noeudCritique);
    void _zigZigDroite(NoeudDictionnaire*& noeudCritique);

    //endregion
};
    
}

#endif /* DICO_H_ */

/**
 * \file Dictionnaire.cpp
 * \brief Ce fichier contient une implantation des méthodes de la classe Dictionnaire
 * \author IFT-2008, Étudiant(e)
 * \version 0.1
 * \date juillet 2020
 *
 */

#include "Dictionnaire.h"

using namespace std;

// Limite du nombre de suggestions
#define LIMITE_SUGGESTIONS 5

namespace TP3
{
       
	/**
     * \fn Dictionnaire::Dictionnaire(std::ifstream &fichier)
     *
     * \param[in] fichier Le fichier à partir duquel on construit le dictionnaire
     */
	Dictionnaire::Dictionnaire(std::ifstream &fichier): racine(nullptr), cpt(0)
    {
        if (fichier)
        {
            for( std::string ligneDico; getline( fichier, ligneDico); )
            {
                if (ligneDico[0] != '#') //Élimine les lignes d'en-tête
                {
                    // Le mot anglais est avant la tabulation (\t).
                    std::string motAnglais = ligneDico.substr(0,ligneDico.find_first_of('\t'));
                    
                    // Le reste (définition) est après la tabulation (\t).
                    std::string motTraduit = ligneDico.substr(motAnglais.length()+1, ligneDico.length()-1);

                    //On élimine tout ce qui est entre crochets [] (possibilité de 2 ou plus)
                    std::size_t pos = motTraduit.find_first_of('[');
                    while (pos!=std::string::npos)
                    {
                        std::size_t longueur_crochet = motTraduit.find_first_of(']')-pos+1;
                        motTraduit.replace(pos, longueur_crochet, "");
                        pos = motTraduit.find_first_of('[');
                    }
                    
                    //On élimine tout ce qui est entre deux parenthèses () (possibilité de 2 ou plus)
                    pos = motTraduit.find_first_of('(');
                    while (pos!=std::string::npos)
                    {
                        std::size_t longueur_crochet = motTraduit.find_first_of(')')-pos+1;
                        motTraduit.replace(pos, longueur_crochet, "");
                        pos = motTraduit.find_first_of('(');
                    }

                    //Position d'un tilde, s'il y a lieu
                    std::size_t posT = motTraduit.find_first_of('~');
                    
                    //Position d'un tilde, s'il y a lieu
                    std::size_t posD = motTraduit.find_first_of(':');
                    
                    if (posD < posT)
                    {
                        //Quand le ':' est avant le '~', le mot français précède le ':'
                        motTraduit = motTraduit.substr(0, posD);
                    }
                    
                    else
                    {
                        //Quand le ':' est après le '~', le mot français suit le ':'
                        if (posT < posD)
                        {
                            motTraduit = motTraduit.substr(posD, motTraduit.find_first_of("([,;\n", posD));
                        }
                        else
                        {
                            //Quand il n'y a ni ':' ni '~', on extrait simplement ce qu'il y a avant un caractère de limite
                            motTraduit = motTraduit.substr(0, motTraduit.find_first_of("([,;\n"));
                        }
                    }
                    
                    //On ajoute le mot au dictionnaire
                    ajouteMot(motAnglais, motTraduit);
                    //std::cout<<motAnglais << " - " << motTraduit<<std::endl;
                }
            }
        }
	}

    Dictionnaire::Dictionnaire() : cpt(0), racine(nullptr) {

    }

    Dictionnaire::~Dictionnaire() {
        _deleteRecursive(racine);
    }

    void Dictionnaire::ajouteMot(const std::string &motOriginal, const std::string &motTraduit) {

        _addRecursive(racine, motOriginal, motTraduit);
    }

    void Dictionnaire::supprimeMot(const std::string &motOriginal) {
        //Supprimer un mot et équilibrer l'arbre AVL
        //Si le mot appartient au dictionnaire, on l'enlève et on équilibre. Sinon, on ne fait rien.
        //Exception	logic_error si l'arbre est vide
        //Exception	logic_error si le mot n'appartient pas au dictionnaire
    }

    double Dictionnaire::similitude(const std::string &mot1, const std::string &mot2) {
        //Quantifier la similitude entre 2 mots (dans le dictionnaire ou pas)
        //Ici, 1 représente le fait que les 2 mots sont identiques, 0 représente le fait que les 2 mots sont complètements différents
        //On retourne une valeur entre 0 et 1 quantifiant la similarité entre les 2 mots donnés
        //Vous pouvez utiliser par exemple la distance de Levenshtein, mais ce n'est pas obligatoire !
        return 0;
    }

    std::vector<std::string> Dictionnaire::suggereCorrections(const std::string &motMalEcrit) {
        //Suggère des corrections pour le mot motMalEcrit sous forme d'une liste de mots, dans un vector, à partir du dictionnaire
        //S'il y a suffisament de mots, on redonne 5 corrections possibles au mot donné. Sinon, on en donne le plus possible
        //Exception	logic_error si le dictionnaire est vide
        return std::vector<std::string>();
    }

    std::vector<std::string> Dictionnaire::traduit(const std::string &mot) {
        //Trouver les traductions possibles d'un mot
        //Si le mot appartient au dictionnaire, on retourne le vecteur des traductions du mot donné.
        //Sinon, on retourne un vecteur vide
        return std::vector<std::string>();
    }

    bool Dictionnaire::appartient(const std::string &data) {
        return _appartientRecursive(racine, data) != nullptr;
    }

    bool Dictionnaire::estVide() const {
        return racine == nullptr;
    }

    //region private methods

    void Dictionnaire::_deleteRecursive(Dictionnaire::NoeudDictionnaire *&arbre) {

        if (arbre != nullptr) {
            _deleteRecursive(arbre->gauche);
            _deleteRecursive(arbre->droite);
            delete arbre;
            arbre = nullptr;
        }
    }

    void Dictionnaire::_addRecursive(NoeudDictionnaire*& node, const std::string &motOriginal, const std::string &motTraduit) {

        if (node == nullptr) {
            node = new NoeudDictionnaire(motOriginal, motTraduit);
            cpt++;
            return;
        } else if (similitude(node->mot, motOriginal) == 1) {
            if (!_vecteurContient(node->traductions, motTraduit)) {
                node->traductions.push_back(motTraduit);
            }
        } else if (node->mot.compare(motOriginal) < 0) {
            _addRecursive(node->droite, motOriginal, motTraduit);
        } else {
            _addRecursive(node->gauche, motOriginal, motTraduit);
        }

        _updateHauteurNoeud(node);
        _balancerUnNoeud(node);
    }

    Dictionnaire::NoeudDictionnaire *
    Dictionnaire::_appartientRecursive(Dictionnaire::NoeudDictionnaire* const &node, const std::string &data) {
	    if (node == nullptr)
            return nullptr;
	    if (similitude(node->mot, data) == 1)
	        return node;

        if (node->mot > data) {
            return _appartientRecursive(node->gauche, data);
        } else {
            return _appartientRecursive(node->droite, data);
        }
    }

    bool Dictionnaire::_vecteurContient(const std::vector<std::string> &vecteur, const std::string& element) {
        for (const auto & iter : vecteur) {
            if (similitude(iter, element) == 1) {
                return true;
            }
        }

        return false;
    }

    void Dictionnaire::_updateHauteurNoeud(Dictionnaire::NoeudDictionnaire *&node) {
        if (node != nullptr) {
            node->hauteur = 1 + max(_hauteur(node->gauche), _hauteur(node->droite));
        }
    }

    int Dictionnaire::_hauteur(Dictionnaire::NoeudDictionnaire *&node) {
        if (node == nullptr)
            return -1;
        return node->hauteur;
    }

    void Dictionnaire::_balancerUnNoeud(NoeudDictionnaire*& node) {

        if (node == nullptr)
            return;

        if (_debalancementAGauche(node)) {
            if (_sousArbrePencheADroite(node->gauche)) {
                _zigZagGauche(node);
            } else {
                _zigZigGauche(node);
            }
        } else if (_debalancementADroite(node)) {
            if (_sousArbrePencheAGauche(node->droite)) {
                _zigZagDroite(node);
            } else {
                _zigZigDroite(node);
            }
        }
    }

    bool Dictionnaire::_debalancementAGauche(Dictionnaire::NoeudDictionnaire *&node) {
        return false;
    }

    bool Dictionnaire::_debalancementADroite(Dictionnaire::NoeudDictionnaire *&node) {
        return false;
    }

    bool Dictionnaire::_sousArbrePencheAGauche(Dictionnaire::NoeudDictionnaire *&node) {
        return false;
    }

    bool Dictionnaire::_sousArbrePencheADroite(Dictionnaire::NoeudDictionnaire *&node) {
        return false;
    }

    void Dictionnaire::_zigZagGauche(Dictionnaire::NoeudDictionnaire *&node) {

    }

    void Dictionnaire::_zigZigGauche(Dictionnaire::NoeudDictionnaire *&node) {

    }

    void Dictionnaire::_zigZigDroite(Dictionnaire::NoeudDictionnaire *&node) {

    }

    void Dictionnaire::_zigZagDroite(Dictionnaire::NoeudDictionnaire *&node) {

    }


    //endregion


  
}//Fin du namespace

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
        _detruireRecursif(racine);
    }

    void Dictionnaire::ajouteMot(const std::string &motOriginal, const std::string &motTraduit) {

        _ajouterRecursif(racine, motOriginal, motTraduit);
    }

    void Dictionnaire::supprimeMot(const std::string &motOriginal) {

	    if (estVide())
	        throw logic_error("arbre est vide");
	    if (!appartient(motOriginal))
	        throw logic_error("Impossible de supprimer un mot qui n'appartien pas au dictionnaire");
        _supprimerMotRecursif(racine, motOriginal);
    }

    double Dictionnaire::similitude(const std::string &mot1, const std::string &mot2) {
	    if (mot1 == mot2)
	        return 1;

	    double minCharEdit = _distanceLevenshtein(mot1, mot2);

	    if (minCharEdit >= mot1.size()) //plus de charactère à éditer que la longueur du mot
	        return 0;

	    return 1 - minCharEdit/mot1.size();

	}

    unsigned int Dictionnaire::_distanceLevenshtein(const std::string &mot1, const std::string &mot2) {
        const size_t lengthMot1 = mot1.size();
        const size_t lengthMot2 = mot2.size();

        vector<vector<unsigned int>> distanceTable(lengthMot1 + 1, vector<unsigned int>(lengthMot2 + 1));
        distanceTable[0][0] = 0;

        for(unsigned int i = 1; i <= lengthMot1; i++)
            distanceTable[i][0] = i;
        for(unsigned int i = 1; i <= lengthMot2; i++)
            distanceTable[0][i] = i;

        for(unsigned int i = 1; i <= lengthMot1; i++)
            for(unsigned int j = 1; j <= lengthMot2; j++) {
                unsigned int dist1 = distanceTable[i - 1][j] + 1;
                unsigned int dist2 = distanceTable[i][j - 1] + 1;
                unsigned int dist3 = distanceTable[i - 1][j - 1] + (mot1[i - 1] == mot2[j - 1] ? 0 : 1);
                distanceTable[i][j] = min(min(dist1, dist2) , dist3);
            }

        return distanceTable[lengthMot1][lengthMot2];
    }

    std::vector<std::string> Dictionnaire::suggereCorrections(const std::string &motMalEcrit) {

        if (estVide())
            throw logic_error("arbre est vide");

        vector<string> suggestions;
        queue<Dictionnaire::NoeudDictionnaire*> noeudAtraite;
        noeudAtraite.push(racine);

        while (suggestions.size() < 5 && !noeudAtraite.empty()) {
            Dictionnaire::NoeudDictionnaire* suggestion = _trouverRecursif(noeudAtraite.front(), motMalEcrit, SIMILITUDE_MIN_POUR_SUGGESTION);
            if (suggestion != nullptr && !_vecteurContient(suggestions, suggestion->mot)) {
                //ce mot n'a pas encore été mis dans le vecteur de suggestions
                suggestions.push_back(suggestion->mot);
            }

            if (suggestion != nullptr) {
                //on ajoute ses enfants comme prochains noeuds à traiter
                if (suggestion->droite != nullptr)
                    noeudAtraite.push(suggestion->droite);
                if (suggestion->gauche != nullptr)
                    noeudAtraite.push(suggestion->gauche);

            }

            noeudAtraite.pop(); //on retire le noeud traité
        }

        return suggestions;
    }

    std::vector<std::string> Dictionnaire::traduit(const std::string &mot) {
        Dictionnaire::NoeudDictionnaire* noeud = _trouverRecursif(racine, mot, SIMILITUDE_MAX);
        if (noeud == nullptr)
            return vector<std::string>();
        return noeud->traductions;
    }

    bool Dictionnaire::appartient(const std::string &mot) {
        return _trouverRecursif(racine, mot, SIMILITUDE_MAX) != nullptr;
    }

    bool Dictionnaire::estVide() const {
        return racine == nullptr;
    }

    void Dictionnaire::_detruireRecursif(Dictionnaire::NoeudDictionnaire *&arbre) {

        if (arbre != nullptr) {
            _detruireRecursif(arbre->gauche);
            _detruireRecursif(arbre->droite);
            delete arbre;
            arbre = nullptr;
        }
    }

    void Dictionnaire::_ajouterRecursif(NoeudDictionnaire*& node, const std::string &motOriginal, const std::string &motTraduit) {

        if (node == nullptr) {
            node = new NoeudDictionnaire(motOriginal, motTraduit);
            cpt++;
            return;
        } else if (similitude(node->mot, motOriginal) == 1) {
            if (!_vecteurContient(node->traductions, motTraduit)) {
                node->traductions.push_back(motTraduit);
            }
        } else if (_baseEstPlustPetitQue(node->mot, motOriginal)) {
            _ajouterRecursif(node->droite, motOriginal, motTraduit);
        } else {
            _ajouterRecursif(node->gauche, motOriginal, motTraduit);
        }

        _updateHauteurNoeud(node);
        _balancerUnNoeud(node);
    }

    Dictionnaire::NoeudDictionnaire *
    Dictionnaire::_trouverRecursif(Dictionnaire::NoeudDictionnaire* const &node, const std::string &motAtrouver, const double similitudeMinimum) {
	    if (node == nullptr)
            return nullptr;
	    if (similitude(node->mot, motAtrouver) >= similitudeMinimum)
	        return node;  //si ici similitudeMinimum est 1 (valeur max), alors les 2 mots sont le même mot.

        if (node->mot > motAtrouver) {
            return _trouverRecursif(node->gauche, motAtrouver, similitudeMinimum);
        } else {
            return _trouverRecursif(node->droite, motAtrouver, similitudeMinimum);
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
            if (_desequilibreSecondaireADroite(node->gauche)) {
                _zigZagGauche(node); //double rotation nécessaire
            } else {
                _zigZigGauche(node); //simple rotation nécessaire
            }
        } else if (_debalancementADroite(node)) {
            if (_desequilibreSecondaireAGauche(node->droite)) {
                _zigZagDroite(node); //double rotation nécessaire
            } else {
                _zigZigDroite(node); //simple rotation nécessaire
            }
        }
    }

    bool Dictionnaire::_debalancementAGauche(Dictionnaire::NoeudDictionnaire *&node) {
	    if (node == nullptr)
            return false;

	    return _hauteur(node->gauche) - _hauteur(node->droite) >= 2;
    }

    bool Dictionnaire::_debalancementADroite(Dictionnaire::NoeudDictionnaire *&node) {
        if (node == nullptr)
            return false;

        return _hauteur(node->droite) - _hauteur(node->gauche) >= 2;
    }

    bool Dictionnaire::_desequilibreSecondaireAGauche(Dictionnaire::NoeudDictionnaire *&node) {
        if (node == nullptr)
            return false;

        return _hauteur(node->gauche) > _hauteur(node->droite);
    }

    bool Dictionnaire::_desequilibreSecondaireADroite(Dictionnaire::NoeudDictionnaire *&node) {
        if (node == nullptr)
            return false;

        return _hauteur(node->droite) > _hauteur(node->gauche);
    }

    void Dictionnaire::_zigZagGauche(Dictionnaire::NoeudDictionnaire *&noeudCritique) {
        _zigZigDroite(noeudCritique->gauche);
        _zigZigGauche(noeudCritique);
    }

    void Dictionnaire::_zigZagDroite(Dictionnaire::NoeudDictionnaire *&noeudCritique) {
        _zigZigGauche(noeudCritique->droite);
        _zigZigDroite(noeudCritique);
    }

    void Dictionnaire::_zigZigGauche(Dictionnaire::NoeudDictionnaire *&noeudCritique) {

	    Dictionnaire::NoeudDictionnaire* noeudCritiqueSecondaire = noeudCritique->gauche;
        noeudCritique->gauche = noeudCritiqueSecondaire->droite;
        noeudCritiqueSecondaire->droite = noeudCritique;

        _updateHauteurNoeud(noeudCritique);
        _updateHauteurNoeud(noeudCritiqueSecondaire);

        noeudCritique = noeudCritiqueSecondaire;
    }

    void Dictionnaire::_zigZigDroite(Dictionnaire::NoeudDictionnaire *&noeudCritique) {

        Dictionnaire::NoeudDictionnaire* noeudCritiqueSecondaire = noeudCritique->droite;
        noeudCritique->droite = noeudCritiqueSecondaire->gauche;
        noeudCritiqueSecondaire->gauche = noeudCritique;

        _updateHauteurNoeud(noeudCritique);
        _updateHauteurNoeud(noeudCritiqueSecondaire);

        noeudCritique = noeudCritiqueSecondaire;
    }

    void Dictionnaire::_supprimerMotRecursif(Dictionnaire::NoeudDictionnaire *&node, const std::string &motAenlever) {

	    if (_baseEstPlustPetitQue(node->mot, motAenlever)) {
            _supprimerMotRecursif(node->droite, motAenlever);
	    } else if (_baseEstPlustGrandQue(node->mot, motAenlever)) {
            _supprimerMotRecursif(node->gauche, motAenlever);
        } else {
	        //on est sur le noeud avec le mot à supprimer
            if (_hauteur(node) == 0) {
                //feuille
                delete node;
                node = nullptr;
                cpt--;
            } else if (_possedeEnfantUnique(node)) {
                if (node->gauche != nullptr) {
                    _swapNodes(node, node->gauche);
                    _supprimerMotRecursif(node->gauche, motAenlever);
                } else {
                    _swapNodes(node, node->droite);
                    _supprimerMotRecursif(node->droite, motAenlever);
                }
            } else {
                //2 enfants
                Dictionnaire::NoeudDictionnaire* noeudMinSousArbreDroite = _noeudMinimalRecusrif(node->droite);
                _swapNodes(node, noeudMinSousArbreDroite);
            }
        }

        _updateHauteurNoeud(node);
        _balancerUnNoeud(node);
    }

    bool Dictionnaire::_baseEstPlustPetitQue(const std::string &base, const std::string &compare) {
        return base.compare(compare) < 0;
    }

    bool Dictionnaire::_baseEstPlustGrandQue(const std::string &base, const std::string &compare) {
        return base.compare(compare) > 0;
    }

    bool Dictionnaire::_possedeEnfantUnique(NoeudDictionnaire* const & node) {
	    bool enfantGauche = node->gauche != nullptr;
	    bool enfantDroite = node->droite != nullptr;

        return enfantGauche ^ enfantDroite;
    }

    Dictionnaire::NoeudDictionnaire* Dictionnaire::_noeudMinimalRecusrif(NoeudDictionnaire* const & node) {
        if (node->gauche == nullptr) {
            return node;
        }
        return _noeudMinimalRecusrif(node->gauche);
    }

    void Dictionnaire::_swapNodes(Dictionnaire::NoeudDictionnaire *&nodeBase, Dictionnaire::NoeudDictionnaire *&nodeToSwapTo) {
        swap(nodeBase->mot, nodeToSwapTo->mot);
        nodeBase->traductions.swap(nodeToSwapTo->traductions);
    }

}//Fin du namespace

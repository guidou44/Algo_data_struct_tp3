/**
 * \file Dictionnaire.cpp
 * \brief Ce fichier contient une implantation des méthodes de la classe Dictionnaire
 * \author IFT-2008, Guillaume Doucet
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
     * \brief Constructeur du dictionnaire qui charge en mémoire les mots du dictionnaire lues dans un fichier
     * \note le stream de fichier doit être ouvert et fermé par l'appelant
     * \param[in] std::ifstream &fichier, Le fichier à partir duquel on construit le dictionnaire
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

    /**
     * \fn Dictionnaire::Dictionnaire()
     * \brief Constructeur sans paramètre d'un dictionnaire. Initialize un dictionnaire vide
     *
     * L'attribut 'racine' est initialisé avec un pointeur null, i.e. ne pointe vers rien sur le monceau.
     *
     */
    Dictionnaire::Dictionnaire() : cpt(0), racine(nullptr) {}

    /**
     * \fn Dictionnaire::~Dictionnaire()
     * \brief Destructeur d'un dictionnaire
     * \note On utilise pas la méthode 'supprimeMot' ici car celle-ci balance l'arbre AVL à chaque appel, ce qui est inutile pour la destruction
     *
     * Le destructeur appel la méthode récursive '_detruireRecursif' qui libère la mémoire allouée sur le monceau pour chaque noeud.
     *
     */
    Dictionnaire::~Dictionnaire() {
        _detruireRecursif(racine);
    }

    /**
     * \fn void Dictionnaire::ajouteMot(const std::string &motOriginal, const std::string &motTraduit)
     * \brief Permet d'ajouter un mot dans le dictionnaire en gardant l'arbre AVL balancé
     * \param[in] const std::string &motOriginal, le mot à ajouter
     * \param[in] const std::string &motTraduit, la traduction du mot à ajouter
     * \post le dictionnaire aura : un mot de plus || une traduction de plus || rien de plus
     *
     * Appel la fonction récursive '_ajouterRecursif' qui fait tout le travail. Si le mot est déjà dans le dictionnaire,
     * seulement la traduction sera ajoutée aux traductions du mot si elle n'est pas déjà là.
     *
     */
    void Dictionnaire::ajouteMot(const std::string &motOriginal, const std::string &motTraduit) {

        _ajouterRecursif(racine, motOriginal, motTraduit);
    }

    /**
     * \fn void Dictionnaire::supprimeMot(const std::string &motOriginal)
     * \brief Permet de supprimer un mot du dictionnaire en gardant l'arbre AVL balancé
     * \param[in] const std::string &motOriginal, le mot à retirer du dictionnaire
     * \pre Le dictionnaire balancé avec le mot dans un de ses noeuds
     * \post le dictionnaire avec un noeud en moins, toujours balancé
     * \exception logic_error si l'arbre est vide ou si le mot à enlever n'appartient pas à l'arbre
     *
     * Appel la fonction récursive '_supprimerMotRecursif' à partir de la racine.
     *
     */
    void Dictionnaire::supprimeMot(const std::string &motOriginal) {

	    if (estVide())
	        throw logic_error("arbre est vide");
	    if (!appartient(motOriginal))
	        throw logic_error("Impossible de supprimer un mot qui n'appartien pas au dictionnaire");
        _supprimerMotRecursif(racine, motOriginal);
    }

    /**
     * \fn double Dictionnaire::similitude(const std::string &mot1, const std::string &mot2)
     * \brief Permet de mesurer la similitude entre deux mots en utilisant la distance de Levenshtein
     * \param[in] const std::string &mot1, le premier mot dans la comparaison
     * \param[in] const std::string &mot2, le second mot dans la comparaison
     * \return double qui correspond à la similitude entre les 2 mots
     *
     * Si lest mots sont identiques, on retourne immédiatement la valeur de similitude maximale, soit 1. Sinon, on
     * calcul la distance de Levenshtein entre les 2 mots (algorithm pour calculer la similitude entre 2 chaînes de charactères).
     * Cette distance peut se résumer au nombre de modifications de charactère unique à faire pour rendre le second mot identique au premier.
     * Ainsi, si cette distance est plus élevée que le nombre de charactère dans le premier mot (il faut donc changer le mot  au complet),
     * on retourne 0 qui correspond à la similitude minimale. Sinon, on calcul le rapport de la distance sur la grosseur du premier mot, ce qui correspond
     * à la fraction du mot qu'il faut changer. On retourne ensuite la similitude, soit 1 - fraction.
     *
     */
    double Dictionnaire::similitude(const std::string &mot1, const std::string &mot2) {
	    if (mot1 == mot2)
	        return SIMILITUDE_MAX;

	    double minCharEdit = _distanceLevenshtein(mot1, mot2);

	    if (minCharEdit >= mot1.size()) //plus de charactère à éditer que la longueur du mot
	        return 0;

	    return 1 - minCharEdit/mot1.size();

	}

    /**
     * \fn unsigned int Dictionnaire::_distanceLevenshtein(const std::string &mot1, const std::string &mot2)
     * \brief Permet de calculer la distance de Levenshtein entre 2 mots
     * \param[in] const std::string &mot1, le premier mot dans le calcul de la distance
     * \param[in] const std::string &mot2, le second mot dans le calcul de la distance
     * \return unsigned int, un entier positif qui représente la distance de Levenshtein entre les 2 mots
     */
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

    /**
     * \fn std::vector<std::string> Dictionnaire::suggereCorrections(const std::string &motMalEcrit)
     * \brief Permet de trouver des suggestions pour un mot mal orthographié
     * \param[in] const std::string &motMalEcrit, le mot mal écrit pour lequel il faut des suggestions
     * \return std::vector<std::string>, un vecteur qui contient les suggestions.
     * \exception logic_error si le dictionnaire est vide
     *
     * La stratégie est d'utiliser une file d'attente pour le traitement des noeuds. Jusqu'à ce qu'on ne trouve plus de noeud qui respecte le critère de
     * similitude minimale (SIMILITUDE_MIN_POUR_SUGGESTION) ou bien que le vecteur des suggestions soit plein (LIMITE_SUGGESTIONS), on parcours l'arbre pour trouver
     * des mots qui sont similaires. On commence par la racine, ensuite dès qu'un noeud est similaire, on l'ajoute au suggestions, puis on met ses enfants dans la file des prochains
     * noeud à traiter. On recommence ensuite la boucle en prenant le premier noeud à traiter dans la file d'attente.
     *
     */
    std::vector<std::string> Dictionnaire::suggereCorrections(const std::string &motMalEcrit) {

        if (estVide())
            throw logic_error("arbre est vide");

        vector<string> suggestions;
        queue<Dictionnaire::NoeudDictionnaire*> noeudAtraite; //file d'attente des noeuds à traiter
        noeudAtraite.push(racine); //on commence par la racine

        while (suggestions.size() < LIMITE_SUGGESTIONS && !noeudAtraite.empty()) {
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

            noeudAtraite.pop(); //on retire le noeud qui vient d'être traité
        }

        return suggestions;
    }

    /**
     * \fn std::vector<std::string> Dictionnaire::traduit(const std::string &mot)
     * \brief Permet de retourner les traductions d'un mot passé en paramètre
     * \param[in] const std::string &mot, le mot à traduire
     * \return std::vector<std::string>, un vecteur qui contient les traductions possibles du mot à traduire. Si le mot n'est pas dans l'arbre, on retourne un vecteur vide.
     */
    std::vector<std::string> Dictionnaire::traduit(const std::string &mot) {
        Dictionnaire::NoeudDictionnaire* noeud = _trouverRecursif(racine, mot, SIMILITUDE_MAX);
        if (noeud == nullptr)
            return vector<std::string>();
        return noeud->traductions;
    }

    /**
     * \fn bool Dictionnaire::appartient(const std::string &mot)
     * \brief Permet de vérifier si un mot appartient au dictionnaire
     * \param[in] const std::string &mot, le mot à vérifier l'appartenance
     * \return bool, true si le mot appartient, false sinon.
     *
     * Cette méthode fait appel à '_trouverRecursif' avec le critère de similitude maximale, soit 1. Ainsi, seul le noeud identique sera retourné.
     *
     */
    bool Dictionnaire::appartient(const std::string &mot) {
        return _trouverRecursif(racine, mot, SIMILITUDE_MAX) != nullptr;
    }

    /**
     * \fn bool Dictionnaire::estVide() const
     * \brief Permet de vérifier si le dictionnaire est vide
     * \return bool, true si le mot dictionnaire est vide, false sinon
     */
    bool Dictionnaire::estVide() const {
        return racine == nullptr;
    }

    /**
     * \fn void Dictionnaire::_detruireRecursif(Dictionnaire::NoeudDictionnaire *&arbre)
     * \brief Permet de libérer récusivement la mémoire du allouée sur le monceau du dictionnaire.
     * \param[in] Dictionnaire::NoeudDictionnaire *&arbre, le noeud à détruire
     *
     * Tant qu'on est pas sur une feuille, cette méthode sera appellée récusivement. Quand on est sur une feuille, celle-ci est retirée de l'arbre.
     * Ainsi de suite jusqu'à ce qu'on ait remonté au noeud premier noeud avec lequel cette méthode a été appellée, typiquement la racine de l'arnre AVL.
     *
     */
    void Dictionnaire::_detruireRecursif(Dictionnaire::NoeudDictionnaire *&arbre) {

        if (arbre != nullptr) {
            _detruireRecursif(arbre->gauche);
            _detruireRecursif(arbre->droite);
            delete arbre;
            arbre = nullptr;
        }
    }

    /**
     * \fn void Dictionnaire::_ajouterRecursif(NoeudDictionnaire*& node, const std::string &motOriginal, const std::string &motTraduit)
     * \brief Permet d'ajouter un mot à un dictionnaire (arbre AVL), à partir d'un noeud de départ, typiquement appellée de la racine.
     * \param[in] NoeudDictionnaire*& node, noeud de départ
     * \param[in] const std::string &motOriginal, mot à ajouter
     * \param[in] const std::string &motTraduit, traduction correspondante
     *
     * Si le mot est déjà présent dans l'arbre, on ajoute simplement sa traduction si elle n'est pas déjà présente. Sinon, on ajoute un nouveau noeud comme feuille,
     * on ajuste les hauteurs des noeuds et on balance l'arbre.
     *
     */
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

    /**
     * \fn Dictionnaire::NoeudDictionnaire* Dictionnaire::_trouverRecursif(Dictionnaire::NoeudDictionnaire* const &node, const std::string &motAtrouver, const double similitudeMinimum)
     * \brief Permet de trouver le noeud d'un mot dans un dictionnaire (arbre AVL) à partir d'un noeud de base avec un critère de similitude minimale.
     * \param[in] Dictionnaire::NoeudDictionnaire* const &node, noeud de départ
     * \param[in] const std::string &motAtrouver, mot à trouver dans le dictionnaire
     * \param[in] const double similitudeMinimum, critère de similitude
     * \return Dictionnaire::NoeudDictionnaire*, un pointeur vers le noeud trouvé, nullptr si noeud pas trouvé.
     *
     *  Cette méthode peut être appellée avec similitudeMinimum = 1 pour trouver un mot spécifique dans le dictionnaire. Elle peut aussi
     *  être appellée avec des critères de similitude plus petits afin de trouver des mots ressemblants.
     *
     */
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

    /**
     * \fn bool Dictionnaire::_vecteurContient(const std::vector<std::string> &vecteur, const std::string& element)
     * \brief Permet de vérifier si un vecteur de std::string contient un element.
     * \param[in] const std::vector<std::string> &vecteur, le vecteur à parcourir.
     * \param[in] const std::string& element, élément à trouver dans le vecteur.
     * \return bool, true si l'élément est dans le vecteur, false sinon
     */
    bool Dictionnaire::_vecteurContient(const std::vector<std::string> &vecteur, const std::string& element) {
        for (const auto & mot : vecteur) {
            if (similitude(mot, element) == SIMILITUDE_MAX) {
                return true;
            }
        }

        return false;
    }

    /**
     * \fn void Dictionnaire::_updateHauteurNoeud(Dictionnaire::NoeudDictionnaire *&node)
     * \brief Permet d'ajuster la hauteur d'un noeud dans l'arbre.
     * \param[in] Dictionnaire::NoeudDictionnaire *&node, le noeud qu'il faut ajuster la hauteur
     * \pre le noeud a une valeur de son champ hauteur mauvaise
     * \post le noeud a une valeur de son champ hauteur ajustée
     */
    void Dictionnaire::_updateHauteurNoeud(Dictionnaire::NoeudDictionnaire *&node) {
        if (node != nullptr) {
            node->hauteur = 1 + max(_hauteur(node->gauche), _hauteur(node->droite));
        }
    }

    /**
     * \fn int Dictionnaire::_hauteur(Dictionnaire::NoeudDictionnaire *&node)
     * \brief Permet de retourner la hauteur d'un noeud du dictionnaire (arbre AVL)
     * \param[in] Dictionnaire::NoeudDictionnaire *&node, le noeud qu'il faut trouver la hauteur
     * \return int, la hauteur du noeud
     *
     * Si le noeud est vide, i.e. inexistant (nullptr), on retourne -1
     *
     */
    int Dictionnaire::_hauteur(Dictionnaire::NoeudDictionnaire *&node) {
        if (node == nullptr)
            return -1;
        return node->hauteur;
    }

    /**
     * \fn void Dictionnaire::_balancerUnNoeud(NoeudDictionnaire*& node)
     * \brief Permet de balancer l'AVL à partir d'un noeud
     * \param[in] NoeudDictionnaire*& node, un pointeur passé par référence vers le noeud à balancer
     *
     * Si le noeud est vide, i.e. inexistant (nullptr), on ne fait rien.
     * Si débalancement gauche -> ZigZag ou ZigZig gauche, selon déséquilibre secondaire
     * Si débalancement droite -> ZigZag ou ZigZig droite, selon déséquilibre secondaire
     *
     */
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

    /**
     * \fn bool Dictionnaire::_debalancementAGauche(Dictionnaire::NoeudDictionnaire *&node)
     * \brief Détermine si un arbre AVL est débalancé à gauche
     * \param[in] NoeudDictionnaire*& node, un pointeur passé par référence vers le noeud à vérifier le débalancement
     * \return true si le neoud est débalancé à gauche, false sinon.
     *
     * Si le noeud est vide, i.e. inexistant (nullptr), on retourne false.
     *
     */
    bool Dictionnaire::_debalancementAGauche(Dictionnaire::NoeudDictionnaire *&node) {
	    if (node == nullptr)
            return false;

	    return _hauteur(node->gauche) - _hauteur(node->droite) >= 2;
    }

    /**
     * \fn bool Dictionnaire::_debalancementADroite(Dictionnaire::NoeudDictionnaire *&node)
     * \brief Détermine si un arbre AVL est débalancé à droite
     * \param[in] NoeudDictionnaire*& node, un pointeur passé par référence vers le noeud à vérifier le débalancement
     * \return true si le neoud est débalancé à droite, false sinon.
     *
     * Si le noeud est vide, i.e. inexistant (nullptr), on retourne false.
     *
     */
    bool Dictionnaire::_debalancementADroite(Dictionnaire::NoeudDictionnaire *&node) {
        if (node == nullptr)
            return false;

        return _hauteur(node->droite) - _hauteur(node->gauche) >= 2;
    }

    /**
     * \fn bool Dictionnaire::_desequilibreSecondaireAGauche(Dictionnaire::NoeudDictionnaire *&node)
     * \brief Détermine si un arbre AVL débalancé a un déséquilibre secondaire à gauche (s'il penche à gauche)
     * \param[in] NoeudDictionnaire*& node, un pointeur passé par référence vers le noeud à vérifier le déséquilibre secondaire
     * \return true le sous-arbre penche à gauche, false sinon.
     *
     * Si le noeud est vide, i.e. inexistant (nullptr), on retourne false.
     *
     */
    bool Dictionnaire::_desequilibreSecondaireAGauche(Dictionnaire::NoeudDictionnaire *&node) {
        if (node == nullptr)
            return false;

        return _hauteur(node->gauche) > _hauteur(node->droite);
    }

    /**
     * \fn bool Dictionnaire::_desequilibreSecondaireADroite(Dictionnaire::NoeudDictionnaire *&node)
     * \brief Détermine si un arbre AVL débalancé a un déséquilibre secondaire à droie (s'il penche à droie)
     * \param[in] NoeudDictionnaire*& node, un pointeur passé par référence vers le noeud à vérifier le déséquilibre secondaire
     * \return true le sous-arbre penche à droie, false sinon.
     *
     * Si le noeud est vide, i.e. inexistant (nullptr), on retourne false.
     *
     */
    bool Dictionnaire::_desequilibreSecondaireADroite(Dictionnaire::NoeudDictionnaire *&node) {
        if (node == nullptr)
            return false;

        return _hauteur(node->droite) > _hauteur(node->gauche);
    }

    /**
     * \fn void Dictionnaire::_zigZagGauche(Dictionnaire::NoeudDictionnaire *&noeudCritique)
     * \brief Double rotation du sous-arbre AVL à partir d'un noeud critique
     * \param[in] Dictionnaire::NoeudDictionnaire *&noeudCritique, le noeud critique à partir du quel on effectue les rotations.
     *
     * ZigZig droite sur enfant gauche suivit de ZigZig gauche, ordre est important.
     *
     */
    void Dictionnaire::_zigZagGauche(Dictionnaire::NoeudDictionnaire *&noeudCritique) {
        _zigZigDroite(noeudCritique->gauche);
        _zigZigGauche(noeudCritique);
    }

    /**
     * \fn void Dictionnaire::_zigZagDroite(Dictionnaire::NoeudDictionnaire *&noeudCritique)
     * \brief Double rotation du sous-arbre AVL à partir d'un noeud critique
     * \param[in] Dictionnaire::NoeudDictionnaire *&noeudCritique, le noeud critique à partir du quel on effectue les rotations.
     *
     * ZigZig gauche sur enfant droite suivit de ZigZig droite, ordre est important.
     *
     */
    void Dictionnaire::_zigZagDroite(Dictionnaire::NoeudDictionnaire *&noeudCritique) {
        _zigZigGauche(noeudCritique->droite);
        _zigZigDroite(noeudCritique);
    }

    /**
     * \fn void Dictionnaire::_zigZigGauche(Dictionnaire::NoeudDictionnaire *&noeudCritique)
     * \brief Simple rotation du sous-arbre AVL à partir d'un noeud critique
     * \param[in] Dictionnaire::NoeudDictionnaire *&noeudCritique, le noeud critique à partir du quel on effectue la rotation
     *
     * La hauteur des noeuds est ajustée à la fin de la méthode.
     *
     */
    void Dictionnaire::_zigZigGauche(Dictionnaire::NoeudDictionnaire *&noeudCritique) {

	    Dictionnaire::NoeudDictionnaire* noeudCritiqueSecondaire = noeudCritique->gauche;
        noeudCritique->gauche = noeudCritiqueSecondaire->droite;
        noeudCritiqueSecondaire->droite = noeudCritique;

        _updateHauteurNoeud(noeudCritique);
        _updateHauteurNoeud(noeudCritiqueSecondaire);

        noeudCritique = noeudCritiqueSecondaire;
    }

    /**
     * \fn void Dictionnaire::_zigZigDroite(Dictionnaire::NoeudDictionnaire *&noeudCritique)
     * \brief Simple rotation du sous-arbre AVL à partir d'un noeud critique
     * \param[in] Dictionnaire::NoeudDictionnaire *&noeudCritique, le noeud critique à partir du quel on effectue la rotation
     *
     * La hauteur des noeuds est ajustée à la fin de la méthode.
     *
     */
    void Dictionnaire::_zigZigDroite(Dictionnaire::NoeudDictionnaire *&noeudCritique) {

        Dictionnaire::NoeudDictionnaire* noeudCritiqueSecondaire = noeudCritique->droite;
        noeudCritique->droite = noeudCritiqueSecondaire->gauche;
        noeudCritiqueSecondaire->gauche = noeudCritique;

        _updateHauteurNoeud(noeudCritique);
        _updateHauteurNoeud(noeudCritiqueSecondaire);

        noeudCritique = noeudCritiqueSecondaire;
    }

    /**
     * \fn void Dictionnaire::_supprimerMotRecursif(Dictionnaire::NoeudDictionnaire *&node, const std::string &motAenlever)
     * \brief Permet de supprimer un mot dans le dictionnaire en utilisant la récursion et en gardant l'arbre AVL balancé.
     * \param[in] Dictionnaire::NoeudDictionnaire *&node, noeud de départ.
     * \param[in] const std::string &motAenlever, le mot du noeud à enlever
     *
     * Après avoir retiré le noeud de l'arbre AVL, les hauteurs des noeuds sont ajustés et l'arbre est rebalancé.
     *
     */
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

    /**
     * \fn bool Dictionnaire::_baseEstPlustPetitQue(const std::string &base, const std::string &compare)
     * \brief Permet de comparer 2 mots pour savoir sit la base est plus petit que le mot comparé
     * \param[in] const std::string &base, la base de la comparaison
     * \param[in] const std::string &compare, le mot comparé à la base
     * \return bool, true si la base est plus petite, false sinon
     *
     * Selon la théorie des arbres AVL, l'enfant gauche d'un noeud est toujours plus petit que le noeud et l'enfant droit est
     * toujours plus grand que le noeud. Bien que ce principe s'applique bien aux chiffres, il faut l'appliquer aux mots. Pour ce faire,
     * la comparaison lexicale et utilisée. Ainsi, un mot est plus petit si sa première lettre est en premier dans l'alphabet. Tant que les lettres sont
     * identiques entre les 2 mots, la prochaine lettre est comparé. La comparaison est encapsulée dans une méthode pour mieu exprimer l'intention du programmeur.
     *
     */
    bool Dictionnaire::_baseEstPlustPetitQue(const std::string &base, const std::string &compare) {
        return base.compare(compare) < 0;
    }

    /**
     * \fn bool Dictionnaire::_baseEstPlustGrandQue(const std::string &base, const std::string &compare)
     * \brief Permet de comparer 2 mots pour savoir sit la base est plus grande que le mot comparé
     * \param[in] const std::string &base, la base de la comparaison
     * \param[in] const std::string &compare, le mot comparé à la base
     * \return bool, true si la base est plus grande, false sinon
     *
     * Selon la théorie des arbres AVL, l'enfant gauche d'un noeud est toujours plus petit que le noeud et l'enfant droit est
     * toujours plus grand que le noeud. Bien que ce principe s'applique bien aux chiffres, il faut l'appliquer aux mots. Pour ce faire,
     * la comparaison lexicale et utilisée. Ainsi, un mot est plus petit si sa première lettre est en premier dans l'alphabet. Tant que les lettres sont
     * identiques entre les 2 mots, la prochaine lettre est comparé. La comparaison est encapsulée dans une méthode pour mieu exprimer l'intention du programmeur.
     *
     */
    bool Dictionnaire::_baseEstPlustGrandQue(const std::string &base, const std::string &compare) {
        return base.compare(compare) > 0;
    }

    /**
     * \fn bool Dictionnaire::_possedeEnfantUnique(NoeudDictionnaire* const & node)
     * \brief Permet de vérifier si un noeud a un seul enfant, gauche ou droite
     * \param[in] NoeudDictionnaire* const & node, le noeud qu'il faut vérifier le nombre d'enfants
     * \return bool, true si le noeud a un seul enfant, false sinon
     */
    bool Dictionnaire::_possedeEnfantUnique(NoeudDictionnaire* const & node) {
	    bool enfantGauche = node->gauche != nullptr;
	    bool enfantDroite = node->droite != nullptr;

        return enfantGauche ^ enfantDroite; //XOR
    }

    /**
     * \fn Dictionnaire::NoeudDictionnaire* Dictionnaire::_noeudMinimalRecusrif(NoeudDictionnaire* const & node)
     * \brief Permet de trouver le noeud minimal dans un arbre AVL à partir d'un noeud de base.
     * \param[in] NoeudDictionnaire* const & node, le noeud de départ
     * \return Dictionnaire::NoeudDictionnaire*, un pointeur vers le noeud minimal trouvé
     *
     * La récusion est utilisée. Le noeud minimal sera la première feuille gauche rencontrée.
     *
     */
    Dictionnaire::NoeudDictionnaire* Dictionnaire::_noeudMinimalRecusrif(NoeudDictionnaire* const & node) {
        if (node->gauche == nullptr) {
            return node;
        }
        return _noeudMinimalRecusrif(node->gauche);
    }

    /**
     * \fn void Dictionnaire::_swapNodes(Dictionnaire::NoeudDictionnaire *&nodeBase, Dictionnaire::NoeudDictionnaire *&nodeToSwapTo)
     * \brief Permet d'échanger 2 noeud dans l'arbre AVL.
     * \param[in] Dictionnaire::NoeudDictionnaire *&nodeBase, le noeud de base dans le swap
     * \param[in] Dictionnaire::NoeudDictionnaire *&nodeToSwapTo, le second noeud dans le swap
     *
     * Le swap change la donnée du noeud, i.e. le mot, ainsi que ses traductions.
     *
     */
    void Dictionnaire::_swapNodes(Dictionnaire::NoeudDictionnaire *&nodeBase, Dictionnaire::NoeudDictionnaire *&nodeToSwapTo) {
        swap(nodeBase->mot, nodeToSwapTo->mot);
        nodeBase->traductions.swap(nodeToSwapTo->traductions);
    }

}//Fin du namespace

/**
 * \file Principal.cpp
 * \brief Fichier de chargement pour le dictionnaire, avec interface de traduction
 * \author IFT-2008, Guillaume Doucet
 * \version 0.1
 * \date juillet 2020
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include "Dictionnaire.h"



using namespace std;
using namespace TP3;

int main(int argc, char* argv[])
{
    const std::string NON_TRADUISABLE = "NON-TRADUISABLE";

	try
	{

		string reponse = "";
		ifstream englishFrench;

		bool reponse_lue = false; //Booléen pour savoir que l'utilisateur a saisi une réponse

		//Tant que la réponse est vide ou non valide, on redemande.
		while (!reponse_lue)
		{
			cout << "Entrez le nom du fichier du dictionnaire Anglais-Francais : ";
			getline(cin, reponse);
			englishFrench.open(reponse.c_str());
			if (englishFrench) {
				cout << "Fichier '" << reponse << "' lu!" << endl;
				reponse_lue = true;
			}
			else {
				cout << "Fichier '" << reponse << "' introuvable! Veuillez entrer un nom de fichier, ou son chemin absolu." << endl;
				cin.clear();
				cin.ignore();
			}
		}

		//Initialisation d'un dictionnaire, avec le constructeur qui utilise un fstream
		Dictionnaire dictEnFr(englishFrench);
		englishFrench.close();


	    // Affichage du dictionnaire niveau par niveau
	    cout << dictEnFr << endl;

		vector<string> motsAnglais; //Vecteur qui contiendra les mots anglais de la phrase entrée

		//Lecture de la phrase en anglais
		cout << "Entrez un texte en anglais (pas de majuscules ou de ponctuation/caracteres speciaux" << endl;
		getline(cin, reponse);

		stringstream reponse_ss(reponse);
		string str_temp;

		//On ajoute un mot au vecteur de mots tant qu'on en trouve dans la phrase (séparateur = espace)
		while (reponse_ss >> str_temp)
		{
			motsAnglais.push_back(str_temp);
		}

		vector<string> motsFrancais; //Vecteur qui contiendra les mots traduits en français

		for (auto & motAnglais : motsAnglais)
			// Itération dans les mots anglais de la phrase donnée
		{
			if (!dictEnFr.appartient(motAnglais)) {
			    vector<string> suggestions = dictEnFr.suggereCorrections(motAnglais);
			    if (suggestions.empty()) {
                    cout << "aucune suggestion possible pour ce mot" << endl;
                    motsFrancais.push_back(NON_TRADUISABLE);
                    continue;
			    }

			    int suggCounter = 1;
                cout << "Le mot '" << motAnglais << "' n'existe pas dans le dictionnaire. Veuillez choisir une des suggestions suivantes :" << endl;
			    for (const string& suggestion : suggestions) {
                    cout << suggCounter << ". " << suggestion << endl;
                    suggCounter++;
			    }

                int choix = 1;

                do
                {
                    cout << endl;
                    cout << "Votre choix : ";
                    cin >> choix;
                    while (cin.fail())
                    {
                        cin.clear(); // clear input buffer to restore cin to a usable state
                        cin.ignore(INT32_MAX, '\n'); // ignore last input
                        cout << "Ce n'est pas un nombre!" << endl;
                        cout << "Votre choix : ";
                        cin >> choix;
                    }
                    if(choix < 1 || choix > suggestions.size())
                        cout << "***Option invalide!***\n";
                }while(choix < 1 || choix > suggestions.size());
                cout << endl;

                motAnglais = suggestions[choix - 1];
			}

			vector<string> traductions = dictEnFr.traduit(motAnglais);

			if (traductions.size() == 1) {
			    motsFrancais.push_back(traductions[0]);
			    continue;
			} else if (traductions.size() == 0) {
                motsFrancais.push_back(NON_TRADUISABLE);
                continue;
			}


            int counter = 1;
			cout << "Plusieurs traductions sont possibles pour le mot '" << motAnglais << "'. Veuillez en choisir une parmi les suivantes :" << endl;
            for (const string& traduction : traductions) {
                cout << counter << ". " << traduction << endl;
                counter++;
            }

            int choix = 1;

            do
            {
                cout << endl;
                cout << "Votre choix : ";
                cin >> choix;
                while (cin.fail())
                {
                    cin.clear(); // clear input buffer to restore cin to a usable state
                    cin.ignore(INT32_MAX, '\n'); // ignore last input
                    cout << "Ce n'est pas un nombre!" << endl;
                    cout << "Votre choix : ";
                    cin >> choix;
                }
                if(choix < 1 || choix > traductions.size())
                    cout << "***Option invalide!***\n";
            }while(choix < 1 || choix > traductions.size());
            cout << endl;

            motsFrancais.push_back(traductions[choix - 1]);
		}

		stringstream phraseFrancais; // On crée un string contenant la phrase,
									 // À partir du vecteur de mots traduits.
		for (vector<string>::const_iterator i = motsFrancais.begin(); i != motsFrancais.end(); i++)
		{
			phraseFrancais << *i << " ";
		}

		cout << "La phrase en francais est :" << endl << phraseFrancais.str() << endl;

	}
	catch (exception & e)
	{
		cerr << e.what() << endl;
	}

	return 0;
}
#pragma warning( disable : 4996 ) 

 
#include <cstdlib>
#include <vector>
#include <iostream>
#include <string>
#include "G2D.h"
#include <math.h>
#define calltologicpersec 50
using namespace std;

// touche P   : mets en pause
// touche ESC : ferme la fenêtre et quitte le jeu


///////////////////////////////////////////////////////////////////////////////
//
//    Données du jeu - structure instanciée dans le main
struct Cible
{
	//vector<V2> VecteurLignes{ V2(0, 310), V2(200, 240),V2(0, 240),V2(200, 180),V2(0, 170),V2(200, 120),V2(0, 100), V2(200, 60),V2(600, 310), V2(400, 240) ,V2(600, 240), V2(400, 180),
	//V2(600, 170), V2(400, 120),V2(600, 100), V2(400, 60) };
	V2      DebutLigne;
	V2      FinLigne;
	bool	CibleActive = true;


	

};
struct Bumper {
	V2 BumperPos;
	int RayonBumper = 40;
	double T0 = calltologicpersec;
	bool draw = false;
	double RBumperMax = 90.0;
	
};
struct GameData
{
	int     idFrame    = 0;
	int     HeightPix  = 800;          // hauteur de la fenêtre de jeu
	int     WidthPix   = 600;          // largeur de la fenêtre de jeu
	V2      BallPos    = V2(100, 100);
	V2      BallMove;
	int     BallRadius = 15; 
	int CompteurG=0;
	int CompteurD=0;
	int Points=0;
	Bumper B;
	Cible C;
	
	vector<V2> PreviousPos;  // stocke les dernières positions de la boule
	
	// bords du flipper
	vector<V2> LP{ V2(595, 550), V2(585, 596), V2(542, 638), V2(476, 671), V2(392, 692), V2(300, 700), V2(207, 692),
		V2(123, 671), V2(57, 638), V2(14, 596), V2(5, 550), V2(5,5), V2(595,5), V2(595,550)};

	vector <Bumper> Bumpers;
	
	
	
	GameData()
	{
		PreviousPos.resize(50); // stocke les 50 dernières positions connues
		BallMove = V2(10,10);   // vecteur déplacement
	}

};

Cible CreeCible(V2 A, V2 B)
{
	Cible C;
	C.DebutLigne = A;
	C.FinLigne = B;

	return C;



}

vector<Cible> CiblesCreeG{ CreeCible(V2(0, 310), V2(200, 240)),	CreeCible(V2(0, 240),V2(200, 180)),CreeCible(V2(0, 170),V2(200, 120)),CreeCible(V2(0, 100), V2(200, 60)) };

vector<Cible>CiblesCreeD{ CreeCible(V2(600, 310), V2(400, 240)), CreeCible(V2(600, 240), V2(400, 180)), CreeCible(V2(600, 170), V2(400, 120)), CreeCible(V2(600, 100), V2(400, 60)) };



// 0 pas d'intersection
// 1/2/3 intersection entre le segment AB et le cercle de rayon r
int CollisionSegCir(V2 A, V2 B, float r, V2 C)
{
	V2 AB = B - A;
	V2 T = AB;
	T.normalize();
	float d = prodScal(T, C - A);
	if (d > 0 && d < AB.norm())
	{
		V2 P = A + d * T; // proj de C sur [AB]
		V2 PC = C - P;
		if (PC.norm() < r) return 2;
		else               return 0;
	}
	if ((C - A).norm() < r) return 1;
	if ((C - B).norm() < r) return 3;
	return 0;
}


V2 Rebond(V2 V, V2 N)
{
	N.normalize();
	V2 T = V2(N.y, -N.x); // rotation de 90° du vecteur n sens horaire
	float vt = prodScal(V,T); // produit scalaire, vt est un nombre
	float vn = prodScal(V,N);   // produit scalaire, vn est un nombre
	V2 R = vt * T - vn * N;// * entre un flottant et un V2
	return R;
}



///////////////////////////////////////////////////////////////////////////////
//
// 
//     fonction de rendu - reçoit en paramètre les données du jeu par référence



void render(const GameData & G)
{

	
    // fond noir	 
	G2D::clearScreen(Color::Black);
	 
	// Titre en haut
	G2D::drawStringFontMono(V2(80, G.HeightPix - 70), string("Super Flipper"), 50, 5, Color::Blue);
	G2D::drawStringFontMono(V2(10,760), to_string(G.Points), 30, 5, Color::Blue);

 
	// la bille
 	G2D::drawCircle(G.BallPos, G.BallRadius, Color::Red, true);
	
	// 3 bumpers

	for (int i=0; i < G.Bumpers.size(); i++)
	{
		G2D::drawCircle(G.Bumpers[i].BumperPos, G.Bumpers[i].RayonBumper, Color::Blue, true);
		if (G.Bumpers[i].draw)
		{

			G2D::drawCircle(G.Bumpers[i].BumperPos, G.Bumpers[i].RBumperMax*G.Bumpers[i].T0/calltologicpersec, Color::Red, true);
			

		}
		
	}
	 
	// les bords

	for (int i = 0; i < G.LP.size()-1; i++)
	   G2D::drawLine(G.LP[i],G.LP[i+1], Color::Green);

	// les positions précédentes

	for (V2 P : G.PreviousPos)
		G2D::setPixel(P, Color::Green);

	// précise que l'on est en pause

	if ( G2D::isOnPause() )
		G2D::drawStringFontMono(V2(100, G.HeightPix / 2), string("Pause..."), 50, 5, Color::Yellow);

	// Trace des cibles a gauche
	
	for (int i = 0; i < CiblesCreeD.size(); i++)
	{
		if (CiblesCreeD[i].CibleActive == true)
		{
			G2D::drawLine(CiblesCreeD[i].DebutLigne, CiblesCreeD[i].FinLigne, Color::Red);
		}
		else
		{
			G2D::drawLine(CiblesCreeD[i].DebutLigne, CiblesCreeD[i].FinLigne, Color::Green);
		}
	}
	
	for (int i = 0; i < CiblesCreeG.size(); i++)
	{
		if (CiblesCreeG[i].CibleActive == true)
		{
			G2D::drawLine(CiblesCreeG[i].DebutLigne, CiblesCreeG[i].FinLigne, Color::Red);
		}
		else
		{
			G2D::drawLine(CiblesCreeG[i].DebutLigne, CiblesCreeG[i].FinLigne, Color::Green);
		}
	}

	// envoie les tracés à l'écran

	G2D::Show();

}

	
///////////////////////////////////////////////////////////////////////////////
//
//
//      Gestion de la logique du jeu - reçoit en paramètre les données du jeu par référence

void CollisionCible(GameData& G)
{
	
	for (int i = 0; i < CiblesCreeG.size(); i++)
	{
		if (G.CompteurD == 4)
		{
			for (int j=0; j < CiblesCreeD.size(); j++)
			{
				CiblesCreeD[j].CibleActive = true;
				G.CompteurD = 0;

			}
		}
		if (G.CompteurG == 4)
		{
			for (int k=0; k < CiblesCreeD.size(); k++)
			{
				CiblesCreeG[k].CibleActive = true;
				G.CompteurG = 0;
			}
		}
			int collisionTypeG = CollisionSegCir(CiblesCreeG[i].DebutLigne, CiblesCreeG[i].FinLigne, G.BallRadius, G.BallPos + G.BallMove);
			int collisionTypeD = CollisionSegCir(CiblesCreeD[i].DebutLigne, CiblesCreeD[i].FinLigne, G.BallRadius, G.BallPos + G.BallMove);
			// GAUCHE
				if ((collisionTypeG == 2) && (CiblesCreeG[i].CibleActive == true))
				{
					G.Points = G.Points + 500;
					V2 AB = CiblesCreeG[i].DebutLigne - CiblesCreeG[i].FinLigne;
					V2 N = V2(AB.y, -AB.x);
					CiblesCreeG[i].CibleActive = false;
					G.BallMove = Rebond(G.BallMove, N);
					G.BallPos = G.BallPos + G.BallMove;
					G.CompteurG++;
				}
				
				else if (collisionTypeG == 1 && (CiblesCreeG[i].CibleActive == true)) {
					
					G.Points = G.Points + 500;
					V2 AB = CiblesCreeG[i].DebutLigne - CiblesCreeG[i].FinLigne;
					G.BallMove = Rebond(G.BallMove, AB);
					G.BallPos = G.BallPos + G.BallMove;
					CiblesCreeG[i].CibleActive = false;
					G.CompteurG++;
			
				}
				else if (collisionTypeG == 3 && (CiblesCreeG[i].CibleActive == true)) {
				
					G.Points = G.Points + 500;
					V2 AB = CiblesCreeG[i].DebutLigne - CiblesCreeG[i].FinLigne;;
					G.BallMove = Rebond(G.BallMove, AB);
					G.BallPos = G.BallPos + G.BallMove;
					CiblesCreeG[i].CibleActive = false;
					G.CompteurG++;
				}

				//DROITE

				else if ((collisionTypeD == 2) && (CiblesCreeD[i].CibleActive == true))
				{
					G.Points = G.Points + 500;
					V2 AB = CiblesCreeD[i].DebutLigne - CiblesCreeD[i].FinLigne;
					V2 N = V2(AB.y, -AB.x);
					G.BallMove = Rebond(G.BallMove, N);
					G.BallPos = G.BallPos + G.BallMove;
					CiblesCreeD[i].CibleActive = false;
					G.CompteurD++;
				}
				
				else if (collisionTypeD == 1 && (CiblesCreeD[i].CibleActive == true)) {
				
					G.Points = G.Points + 500;
					V2 AB = CiblesCreeD[i].DebutLigne - CiblesCreeD[i].FinLigne;
					G.BallMove = Rebond(G.BallMove, AB);
					G.BallPos = G.BallPos + G.BallMove;
					CiblesCreeD[i].CibleActive = false;
					G.CompteurD++;

				}
				else if (collisionTypeD == 3 && (CiblesCreeD[i].CibleActive == true)) {
				
					G.Points = G.Points + 500;
					V2 AB = CiblesCreeD[i].DebutLigne - CiblesCreeD[i].FinLigne;
					G.BallMove = Rebond(G.BallMove, AB);
					G.BallPos = G.BallPos + G.BallMove;
					CiblesCreeD[i].CibleActive = false;
					G.CompteurD++;
				}
	}
}


void CollisionBumper(GameData& G)
{
	
	int RayonBumper = 40;
	double RBumperMax = 90;
	
	for (int i = 0; i < G.Bumpers.size(); i++)
	{
		V2 CC = G.BallPos - G.Bumpers[i].BumperPos;
		if (CC.norm() < G.BallRadius + RayonBumper)
		{
			G.Points = G.Points + 100;
			G.Bumpers[i].draw = true;
			G.Bumpers[i].T0 = calltologicpersec;
			G.BallMove = Rebond(G.BallMove, CC.GetNormalized());
			G.BallPos = G.BallPos + G.BallMove;
			
		}
	}
}

void Logic(GameData& G) {
	G.idFrame += 1;

	for (int i = 0; i < G.LP.size() - 1; i++) {
		int collisionType = CollisionSegCir(G.LP[i + 1], G.LP[i], G.BallRadius, G.BallPos + G.BallMove);
		/*if (collisionType == 1) {
			cout << "1";
			V2 AB = G.LP[i + 1] - G.LP[i];
			Re = Rebond(G.BallMove, AB);
			G.BallMove = Re;
			G.BallPos = G.BallPos + G.BallMove;
		}
		*/
		if (collisionType == 2)
		{

			V2 AB = G.LP[i + 1] - G.LP[i];
			V2 N = V2(AB.y, -AB.x);

			G.BallMove = Rebond(G.BallMove, N);
			G.BallPos = G.BallPos + G.BallMove;


		}
		/*else if (collisionType == 3) {
				cout << "3";
				V2 AB = G.LP[i + 1] - G.LP[i];
				Re = Rebond(G.BallPos, AB);
				G.BallMove = Re;
				G.BallPos = G.BallPos + G.BallMove;

			}*/

	}
	CollisionCible(G);
	CollisionBumper(G);

	for (int i = 0; i < G.Bumpers.size(); i++)
	{
		if (G.Bumpers[i].draw == false) continue;
		G.Bumpers[i].T0--;
		if (G.Bumpers[i].T0 <= 0)
		{

			G.Bumpers[i].draw = false;
		}
	}

	G.BallPos = G.BallPos + G.BallMove;
	G.PreviousPos.push_back(V2(G.BallPos.x, G.BallPos.y));
	G.PreviousPos.erase(G.PreviousPos.begin());
}

///////////////////////////////////////////////////////////////////////////////
//
//
//        Démarrage de l'application


int main(int argc, char* argv[])
{
	
	GameData G;   // instanciation de l'unique objet GameData qui sera passé aux fonctions render et logic

	G2D::initWindow(V2(G.WidthPix, G.HeightPix), V2(200, 200), string("Super Flipper 600 !!"));

	  // si vous réduisez cette valeur => ralentit le jeu
	G.Bumpers.push_back(Bumper());
	G.Bumpers.back().BumperPos = V2(200, 400);
	G.Bumpers.push_back(Bumper());
	G.Bumpers.back().BumperPos = V2(400, 400);
	G.Bumpers.push_back(Bumper());
	G.Bumpers.back().BumperPos = V2(300, 550);
	
	G2D::Run(Logic, render, G, calltologicpersec, true);
}






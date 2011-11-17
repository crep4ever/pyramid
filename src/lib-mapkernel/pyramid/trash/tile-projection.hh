    //******************************************************************************	  
    //Programmation dynamique pour le positionnement des arêtes lors de la projection
    //******************************************************************************
    
    void projection();

    /// Calcule la différence de niveau de gris entre 2 pixels séparés
    // par un doubler
    /// @param  ADoublet : le doublet
    /// @return le gradient
    uint gradient(const CDoublet & ADoublet);
    float gradientInverse(const CDoublet & ADoublet);

    /// Dynamic programming: à partir d'une arête au niveau k
    /// on calcule son plongement au niveau k+1
    /// @param AEdge : l'arête du niveau k
    void processEdge(std::vector<CDoublet> AEdge);

    /// DP: on calcule la matrice hi composée du gradient moyen calculé sur tous
    /// les chemins (droites) entre un élément de ASet1 et ASet2
    /// @param ASet1 : xi
    /// @param ASet2 : xi+1
    CMatrix<uint>* computeGradientTable(const std::vector<CDoublet> & ASet1, 
					const std::vector<CDoublet> & ASet2);

    /// DP: calcule l'ensemble des positions potentielles d'un pointel
    /// du niveau k lorsqu'il est projeté dans k+1. 
    /// @param APointel : le pointel du niveau k
    /// @return le vecteur des posisitions potentielles au niveau k+1
    std::vector<CDoublet> candidatePositions(const CDoublet & APointel);

    /// DP: calcule la droite reliant 2 pointels
    /// @param APointel1 : point de départ
    /// @param APointel2 : point d'arrivée
    /// @param AGradient : calcule le gradient moyen le long de la droite
    /// @param ADraw : allume le chemin dans la matrice
    /// @return : le gradient le long de la droite
    uint computeLine(const CDoublet & APointel1, 
		     const CDoublet & APointel2,
		     bool AGradient, bool ADraw);

    /// Calcule la tangente en un point p
    /// @param AIndex : index du point dans la séquence à dessiner
    void computeTangent(uint AIndex);
    
    void computeSpline(const uint x0, const uint y0,
		       const float u0, const float v0,
		       const uint x1, const uint y1,
		       const float u1, const float v1,
		       const float precision=1) ;
      

    /// DP: Calcul des fonctions de maximisation fi.
    /// Cette matrice correspond aux valeurs max des colonnes des hi
    /// @param AHi : la matrice dans laquelle on va chercher les valeurs max en colonnes
    /// @param ASet2 : xi+1 (colonne 1 de fi)
    /// @param ASet1 : xi (colonne 3 de fi mais trié suivant la colonne 2)
    /// @return la matrice fi
    std::vector<CMaxRow*> computeMaximisationFunction(CMatrix<uint>* AHi, 
					       std::vector<CDoublet> ASet2,
					       std::vector<CDoublet> ASet1);

    /// DP: afin de calculer la matrice fi, on a besoin d'additionner hi avec 
    //// la colonne 2 (valeurs max) de fi-1
    /// @param AHi : les gradients
    /// @param AFi : les valeurs du fi de l'étape précédente
    CMatrix<uint>* updateGradientTable(CMatrix<uint>* AHi, const std::vector<CMaxRow*> & AFi);
    
    /// DP: Récupère les valeurs maximales de chaque colonne de la matrice AHi
    /// @param AHi : la matrice
    /// @return le vecteur des valeurs max (key) associées aux xi-1 adéquats
    std::multimap<uint, CDoublet> getColumnMaxValues(CMatrix<uint>* AHi,
						const std::vector<CDoublet> & ASet1);



    uint computePath(const std::vector<CDoublet> & AEdge, bool ADraw);

    ///Permutte deux pointels de façon à ce que le premier soit plus petit
    ///que le second
    ///@param APointel1 : premier pointel
    ///@param APointel2 : second pointel
    void swapPointels(CDoublet APointel1, CDoublet APointel2);

    bool checkComputeLine();

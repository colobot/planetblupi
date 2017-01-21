// Arrange.cpp
//

#include "DECOR.H"
#include "MISC.H"

// Cette table indique les quarts de cases contenant de
// l'eau lorsque la valeur est � un.
//		0  1
//		2  3
static char tableSee[14*4] =
{
	0,0,0,0,	// 1
	0,1,0,1,	// 2
	0,0,1,1,	// 3
	1,0,1,0,	// 4
	1,1,0,0,	// 5
	0,0,0,1,	// 6
	0,0,1,0,	// 7
	1,0,0,0,	// 8
	0,1,0,0,	// 9
	0,1,1,1,	// 10
	1,0,1,1,	// 11
	1,1,1,0,	// 12
	1,1,0,1,	// 13
	1,1,1,1,	// 14
};

// Cette table indique les quarts de cases contenant de
// la mousse ou de la terre lorsque la valeur est � un.
//		0  1
//		2  3
static char tableDark[13*4] =
{
	1,1,1,1,	// 20
	0,1,0,1,	// 21
	0,0,1,1,	// 22
	1,0,1,0,	// 23
	1,1,0,0,	// 24
	0,0,0,1,	// 25
	0,0,1,0,	// 26
	1,0,0,0,	// 27
	0,1,0,0,	// 28
	1,1,1,0,	// 29
	1,1,0,1,	// 30
	0,1,1,1,	// 31
	1,0,1,1,	// 32
};

// Retourne les bits contenant de l'eau.

BOOL CDecor::GetSeeBits(POINT cel, char *pBits, int index)
{
	int		icon;

	pBits[0] = 0;
	pBits[1] = 0;
	pBits[2] = 0;
	pBits[3] = 0;

	if ( cel.x < 0 || cel.x >= MAXCELX ||
		 cel.y < 0 || cel.y >= MAXCELY )  return FALSE;

	icon = m_decor[cel.x/2][cel.y/2].floorIcon;

	if ( index == 0 )  // eau ?
	{
		if ( icon < 1 || icon > 14 )  return TRUE;
		icon -= 1;
		pBits[0] = tableSee[icon*4+0];
		pBits[1] = tableSee[icon*4+1];
		pBits[2] = tableSee[icon*4+2];
		pBits[3] = tableSee[icon*4+3];
	}

	if ( index == 1 )  // mousse ?
	{
		if ( icon >= 2 && icon <= 14 )  return FALSE;  // eau ?
		if ( icon == 66 || icon == 79 )  // mousse sp�ciale ?
		{
			pBits[0] = 1;
			pBits[1] = 1;
			pBits[2] = 1;
			pBits[3] = 1;
			return TRUE;
		}
		if ( icon < 20 || icon > 32 )  return TRUE;
		icon -= 20;
		pBits[0] = tableDark[icon*4+0];
		pBits[1] = tableDark[icon*4+1];
		pBits[2] = tableDark[icon*4+2];
		pBits[3] = tableDark[icon*4+3];
	}

	if ( index == 2 )  // terre ?
	{
		if ( icon >= 2 && icon <= 14 )  return FALSE;  // eau ?
		if ( (icon >= 46 && icon <= 48) ||  // terre sp�ciale ?
			 icon == 71 )  // terre � fer ?
		{
			pBits[0] = 1;
			pBits[1] = 1;
			pBits[2] = 1;
			pBits[3] = 1;
			return TRUE;
		}
		if ( icon < 33 || icon > 45 )  return TRUE;
		icon -= 33;
		pBits[0] = tableDark[icon*4+0];
		pBits[1] = tableDark[icon*4+1];
		pBits[2] = tableDark[icon*4+2];
		pBits[3] = tableDark[icon*4+3];
	}

	return TRUE;
}

void CopyBits(char *pDst, char *pSrc)
{
	for ( int i=0 ; i<4 ; i++ )
	{
		*pDst++ = *pSrc++;
	}
}

BOOL ChangeBits(char *pDst, char *pSrc)
{
	for ( int i=0 ; i<4 ; i++ )
	{
		if ( *pDst++ != *pSrc++ )  return TRUE;
	}
	return FALSE;
}

// Retourne l'ic�ne correspondant aux bits d'eaux.

int CDecor::GetSeeIcon(char *pBits, int index)
{
	int		i;

	if ( index == 0 )  // eau ?
	{
		for ( i=0 ; i<14 ; i++ )
		{
			if ( tableSee[i*4+0] == pBits[0] &&
				 tableSee[i*4+1] == pBits[1] &&
				 tableSee[i*4+2] == pBits[2] &&
				 tableSee[i*4+3] == pBits[3] )  return i+1;
		}
	}

	if ( index == 1 )  // mousse ?
	{
		for ( i=0 ; i<13 ; i++ )
		{
			if ( tableDark[i*4+0] == pBits[0] &&
				 tableDark[i*4+1] == pBits[1] &&
				 tableDark[i*4+2] == pBits[2] &&
				 tableDark[i*4+3] == pBits[3] )  return i+20;
		}
	}

	if ( index == 2 )  // terre ?
	{
		for ( i=0 ; i<13 ; i++ )
		{
			if ( tableDark[i*4+0] == pBits[0] &&
				 tableDark[i*4+1] == pBits[1] &&
				 tableDark[i*4+2] == pBits[2] &&
				 tableDark[i*4+3] == pBits[3] )  return i+33;
		}
	}

	if ( pBits[0] == 0 &&
		 pBits[1] == 0 &&
		 pBits[2] == 0 &&
		 pBits[3] == 0 )  return 1;  // herbe

	return -1;
}

// Arrange le sol apr�s une modification.

void CDecor::ArrangeFloor(POINT cel)
{
	POINT	test;
	int		max, index, icon;
	char	here[4], bits[4], init[4];
	BOOL	bModif = FALSE;

	icon = m_decor[cel.x/2][cel.y/2].floorIcon;

	if ( icon >= 59 && icon <= 64 )  return;  // pont ?

	max = 3;
	if ( icon >= 15 && icon <= 18 )  // dalle sp�ciale ?
	{
		max = 1;  // s'occupe que de l'eau !
	}

	for ( index=0 ; index<max ; index++ )
	{
		if ( !GetSeeBits(cel, here, index) )  continue;

		test.x = cel.x -2;		// en bas � gauche
		test.y = cel.y +2;
		if ( GetSeeBits(test, bits, index) )
		{
			if ( bits[2] == here[2] &&
				 bits[0] != here[2] &&
				 bits[1] != here[2] &&
				 bits[3] != here[2] )
			{
				here[2] = bits[1];
				bModif = TRUE;
			}
		}

		test.x = cel.x -2;		// en haut � gauche
		test.y = cel.y -2;
		if ( GetSeeBits(test, bits, index) )
		{
			if ( bits[0] == here[0] &&
				 bits[1] != here[0] &&
				 bits[2] != here[0] &&
				 bits[3] != here[0] )
			{
				here[0] = bits[3];
				bModif = TRUE;
			}
		}

		test.x = cel.x +2;		// en haut � droite
		test.y = cel.y -2;
		if ( GetSeeBits(test, bits, index) )
		{
			if ( bits[1] == here[1] &&
				 bits[0] != here[1] &&
				 bits[2] != here[1] &&
				 bits[3] != here[1] )
			{
				here[1] = bits[2];
				bModif = TRUE;
			}
		}

		test.x = cel.x +2;		// en bas � droite
		test.y = cel.y +2;
		if ( GetSeeBits(test, bits, index) )
		{
			if ( bits[3] == here[3] &&
				 bits[0] != here[3] &&
				 bits[1] != here[3] &&
				 bits[2] != here[3] )
			{
				here[3] = bits[0];
				bModif = TRUE;
			}
		}

		if ( bModif )
		{
			icon = GetSeeIcon(here, index);
			if ( icon != -1 )  m_decor[cel.x/2][cel.y/2].floorIcon = icon;
		}


		test.x = cel.x -2;		// � gauche
		test.y = cel.y;
		if ( GetSeeBits(test, bits, index) )
		{
			CopyBits(init, bits);
			bits[1] = here[0]?1:0;
			bits[3] = here[2]?1:0;
			icon = GetSeeIcon(bits, index);
			if ( ChangeBits(init, bits) && icon != -1 )
			{
				m_decor[test.x/2][test.y/2].floorIcon = icon;
			}
		}

		test.x = cel.x -2;		// en haut � gauche
		test.y = cel.y -2;
		if ( GetSeeBits(test, bits, index) )
		{
			CopyBits(init, bits);
			bits[3] = here[0]?1:0;
			icon = GetSeeIcon(bits, index);
			if ( ChangeBits(init, bits) && icon != -1 )
			{
				m_decor[test.x/2][test.y/2].floorIcon = icon;
			}
		}

		test.x = cel.x;			// en haut
		test.y = cel.y -2;
		if ( GetSeeBits(test, bits, index) )
		{
			CopyBits(init, bits);
			bits[2] = here[0]?1:0;
			bits[3] = here[1]?1:0;
			icon = GetSeeIcon(bits, index);
			if ( ChangeBits(init, bits) && icon != -1 )
			{
				m_decor[test.x/2][test.y/2].floorIcon = icon;
			}
		}

		test.x = cel.x +2;		// en haut � droite
		test.y = cel.y -2;
		if ( GetSeeBits(test, bits, index) )
		{
			CopyBits(init, bits);
			bits[2] = here[1]?1:0;
			icon = GetSeeIcon(bits, index);
			if ( ChangeBits(init, bits) && icon != -1 )
			{
				m_decor[test.x/2][test.y/2].floorIcon = icon;
			}
		}

		test.x = cel.x +2;		// � droite
		test.y = cel.y;
		if ( GetSeeBits(test, bits, index) )
		{
			CopyBits(init, bits);
			bits[0] = here[1]?1:0;
			bits[2] = here[3]?1:0;
			icon = GetSeeIcon(bits, index);
			if ( ChangeBits(init, bits) && icon != -1 )
			{
				m_decor[test.x/2][test.y/2].floorIcon = icon;
			}
		}

		test.x = cel.x +2;		// en bas � droite
		test.y = cel.y +2;
		if ( GetSeeBits(test, bits, index) )
		{
			CopyBits(init, bits);
			bits[0] = here[3]?1:0;
			icon = GetSeeIcon(bits, index);
			if ( ChangeBits(init, bits) && icon != -1 )
			{
				m_decor[test.x/2][test.y/2].floorIcon = icon;
			}
		}

		test.x = cel.x;			// en bas
		test.y = cel.y +2;
		if ( GetSeeBits(test, bits, index) )
		{
			CopyBits(init, bits);
			bits[0] = here[2]?1:0;
			bits[1] = here[3]?1:0;
			icon = GetSeeIcon(bits, index);
			if ( ChangeBits(init, bits) && icon != -1 )
			{
				m_decor[test.x/2][test.y/2].floorIcon = icon;
			}
		}

		test.x = cel.x -2;		// en bas � gauche
		test.y = cel.y +2;
		if ( GetSeeBits(test, bits, index) )
		{
			CopyBits(init, bits);
			bits[1] = here[2]?1:0;
			icon = GetSeeIcon(bits, index);
			if ( ChangeBits(init, bits) && icon != -1 )
			{
				m_decor[test.x/2][test.y/2].floorIcon = icon;
			}
		}
	}
}



// Cette table donne les directions dans l'ordre
// est-sud-ouest-nord pour les murs.
static char tableMur[5*15] =
{
	20,	1,0,1,0,
	21,	0,1,0,1,
	22,	1,1,0,0,
	23,	0,1,1,0,
	24,	0,0,1,1,
	25,	1,0,0,1,
	26,	1,1,1,1,

	26, 0,1,1,1,
	26, 1,0,1,1,
	26, 1,1,0,1,
	26, 1,1,1,0,

	20,	1,0,0,0,
	20,	0,0,1,0,
	21,	0,1,0,0,
	21,	0,0,0,1,
};

static short tableMurDir[4*2] =
{
	+2, 0,		// est
	 0,+2,		// sur
	-2, 0,		// ouest
	 0,-2,		// nord
};

// Arrange un mur en fonction des autres murs dans toutes
// les directions.
// index=0 si mur       (20..26)
// index=1 si palissade (65..71)
// index=2 si barri�re  (106..112)

void CDecor::ArrangeMur(POINT cel, int &icon, int index)
{
	int		i, x, y, channel;
	int		first, last, matiere;
	int		icons[4];
	char	murs[4];

	if ( index == 0 )
	{
		first   = 20;
		last    = 26;
		matiere = 44;  // pierres
	}
	if ( index == 1 )
	{
		first   = 65;
		last    = 71;
		matiere = 36;  // planches
	}
	if ( index == 2 )
	{
		first   = 106;
		last    = 112;
		matiere = 36;  // planches
	}

	for ( i=0 ; i<4 ; i++ )
	{
		x = cel.x + tableMurDir[i*2+0];
		y = cel.y + tableMurDir[i*2+1];

		if ( IsValid(GetCel(x,y)) )
		{
			icons[i] = m_decor[x/2][y/2].objectIcon;
			if ( icons[i] == matiere )  // pierres/planches ?
			{
				MoveGetObject(GetCel(x,y), channel, icons[i]);
			}

			if ( icons[i] < first || icons[i] > last )
			{
				icons[i] = -1;
			}
		}
		else
		{
			icons[i] = -1;
		}
	}

	for ( i=0 ; i<4 ; i++ )
	{
		if ( icons[i] == -1 )
		{
			murs[i] = 0;
		}
		else
		{
			murs[i] = tableMur[(icons[i]-first)*5+1+((i+2)%4)];
		}
	}

	for ( i=0 ; i<15 ; i++ )
	{
		if ( murs[0] == tableMur[i*5+1] &&
			 murs[1] == tableMur[i*5+2] &&
			 murs[2] == tableMur[i*5+3] &&
			 murs[3] == tableMur[i*5+4] )
		{
			icon = tableMur[i*5+0];
			icon += first-20;
			return;
		}
	}

	icon = -1;
}

// Arrange les objets avant une construction.

void CDecor::ArrangeBuild(POINT cel, int &channel, int &icon)
{
	int		index, i, x, y;
	int		first, last, matiere;
	int		oldChannel, oldIcon;

	for ( index=0 ; index<3 ; index++ )
	{
		if ( index == 0 )
		{
			first   = 20;
			last    = 26;
			matiere = 44;  // pierres
		}
		if ( index == 1 )
		{
			first   = 65;
			last    = 71;
			matiere = 36;  // planches
		}
		if ( index == 2 )
		{
			first   = 106;
			last    = 112;
			matiere = 36;  // planches
		}

		// Rien � faire si pas mur.
		if ( channel != CHOBJECT || icon != last )  continue;

		oldChannel = m_decor[cel.x/2][cel.y/2].objectChannel;
		oldIcon    = m_decor[cel.x/2][cel.y/2].objectIcon;

		m_decor[cel.x/2][cel.y/2].objectChannel = channel;
		m_decor[cel.x/2][cel.y/2].objectIcon    = icon;

		for ( i=0 ; i<4 ; i++ )
		{
			x = cel.x + tableMurDir[i*2+0];
			y = cel.y + tableMurDir[i*2+1];

			if ( IsValid(GetCel(x,y)) )
			{
				icon = m_decor[x/2][y/2].objectIcon;
				if ( icon == matiere )  // pierres/planches ?
				{
					MoveGetObject(GetCel(x,y), channel, icon);
				}

				if ( icon >= first && icon <= last )
				{
					ArrangeMur(GetCel(x,y), icon, index);

					if ( icon != -1 )
					{
						if ( !MovePutObject(GetCel(x,y), channel, icon) )
						{
							m_decor[x/2][y/2].objectChannel = channel;
							m_decor[x/2][y/2].objectIcon    = icon;
						}
					}
				}
			}
		}

		m_decor[cel.x/2][cel.y/2].objectChannel = oldChannel;
		m_decor[cel.x/2][cel.y/2].objectIcon    = oldIcon;

		ArrangeMur(cel, icon, index);
		if ( icon == -1 )  icon = last;
	}
}

// Arrange les objets apr�s une modification.

void CDecor::ArrangeObject(POINT cel)
{
	int		channel, icon;
	int		first, last;
	int		index, i, j, k, x, y;
	POINT	vector, test, pos;
	BOOL	bTour;

	for ( index=0 ; index<3 ; index++ )
	{
		if ( index == 0 )
		{
			first   = 20;  // murs
			last    = 26;
		}
		if ( index == 1 )
		{
			first   = 65;  // palissades
			last    = 71;
		}
		if ( index == 2 )
		{
			first   = 106;  // barri�re
			last    = 112;
		}

		for ( i=0 ; i<4 ; i++ )
		{
			x = cel.x + tableMurDir[i*2+0];
			y = cel.y + tableMurDir[i*2+1];

			if ( IsValid(GetCel(x,y)) )
			{
				icon = m_decor[x/2][y/2].objectIcon;
				if ( icon >= first && icon <= last )
				{
					ArrangeMur(GetCel(x,y), icon, index);

					if ( icon != -1 )
					{
						m_decor[x/2][y/2].objectChannel = CHOBJECT;
						m_decor[x/2][y/2].objectIcon    = icon;
					}
				}
			}
		}

		if ( m_decor[cel.x/2][cel.y/2].objectIcon == last )
		{
			ArrangeMur(cel, icon, index);
			if ( icon == -1 )  icon = last;

			m_decor[cel.x/2][cel.y/2].objectChannel = CHOBJECT;
			m_decor[cel.x/2][cel.y/2].objectIcon    = icon;
		}
	}

	// Arrange les rayons entre les tours.
	if ( m_decor[cel.x/2][cel.y/2].objectIcon == 27 ||  // tour ?
		 m_decor[cel.x/2][cel.y/2].objectIcon == -1 )   // rien ?
	{
		for ( i=0 ; i<4 ; i++ )
		{
			vector = GetVector(i*2*16);
			test = cel;

			bTour = FALSE;
			j = 0;
			while ( TRUE )
			{
				test.x += vector.x*2;
				test.y += vector.y*2;

				if ( m_decor[test.x/2][test.y/2].objectIcon == 27 )  // tour ?
				{
					bTour = TRUE;
					break;
				}

				if ( m_decor[test.x/2][test.y/2].objectIcon != -1 &&
					 m_decor[test.x/2][test.y/2].objectIcon != 10001-i%2 )
				{
					break;
				}

				j ++;
				if ( j >= 2+1 )  break;
			}

			if ( m_decor[cel.x/2][cel.y/2].objectIcon != 27 )  // pas tour ?
			{
				bTour = FALSE;
			}

			test = cel;
			for ( k=0 ; k<j ; k++ )
			{
				test.x += vector.x*2;
				test.y += vector.y*2;

				if ( bTour )
				{
					channel = CHOBJECT;
					icon = 10001-i%2;  // rayon e-o (10001) ou n-s (10000)
				}
				else
				{
					channel = -1;
					icon    = -1;
				}
				m_decor[test.x/2][test.y/2].objectChannel = channel;
				m_decor[test.x/2][test.y/2].objectIcon    = icon;

				if ( !m_bBuild && bTour )
				{
					if ( MoveCreate(test, -1, FALSE, CHOBJECT,-1,
									-1,-1, 9999,1,0, TRUE) )
					{
						MoveAddIcons(test, 5-i%2, TRUE);  // �clairs
					}

					pos = ConvCelToPos(test);
					m_pSound->PlayImage(SOUND_RAYON1, pos);
				}

				if ( !m_bBuild && !bTour )
				{
					MoveFinish(test);
				}
			}
		}
	}
}



// Test s'il faut remplir le sol ici.

BOOL CDecor::ArrangeFillTestFloor(POINT cel1, POINT cel2)
{
	POINT		cel;
	int			icon1, icon2;

	icon1 = m_fillSearchIcon;
	icon2 = m_fillSearchIcon;

	if ( m_fillPutChannel == CHFLOOR &&
		 m_fillPutIcon    == 1       &&  // met de l'herbe..
		 m_fillSearchIcon == 14      )   // ..sur de l'eau ?
	{
		icon1 = 2;
		icon2 = 14;  // eau & rives
	}

	if ( m_fillPutChannel == CHFLOOR &&
		 m_fillPutIcon    == 14      &&  // met de l'eau..
		 m_fillSearchIcon == 1       )   // ..sur de l'herbe ?
	{
		icon1 = 1;
		icon2 = 13;  // herbe & rives
	}

	for ( cel.x=cel1.x ; cel.x<=cel2.x ; cel.x+=2 )
	{
		for ( cel.y=cel1.y ; cel.y<=cel2.y ; cel.y+=2 )
		{
			if ( !IsValid(cel) )  continue;

			if ( m_decor[cel.x/2][cel.y/2].floorChannel != m_fillSearchChannel ||
				 m_decor[cel.x/2][cel.y/2].floorIcon    <  icon1 ||
				 m_decor[cel.x/2][cel.y/2].floorIcon    >  icon2 )
			{
				return FALSE;
			}

			if ( m_fillPutChannel == CHFLOOR &&
				 m_fillPutIcon    == 14      &&  // met de l'eau ?
				 m_decor[cel.x/2][cel.y/2].objectIcon != -1 )
			{
				return FALSE;
			}
		}
	}

	if ( m_fillPutChannel == CHFLOOR &&
		 m_fillPutIcon    == 14      &&  // met de l'eau ?
		 IsBlupiHereEx(cel1, cel2, -1, FALSE) )
	{
		return FALSE;
	}

	return TRUE;
}

// Test s'il faut remplir ici.

BOOL CDecor::ArrangeFillTest(POINT pos)
{
	POINT		cel1, cel2;

	if ( m_pFillMap[(pos.x/2)+(pos.y/2)*(MAXCELX/2)] == 1 )
	{
		return FALSE;
	}

	if ( m_bFillFloor )
	{
		cel1.x = pos.x-2;
		cel1.y = pos.y-2;
		cel2.x = pos.x+3;
		cel2.y = pos.y+3;
		return ArrangeFillTestFloor(cel1, cel2);
	}
	else
	{
		if ( m_decor[pos.x/2][pos.y/2].objectChannel == m_fillSearchChannel &&
			 m_decor[pos.x/2][pos.y/2].objectIcon    == m_fillSearchIcon    &&
			 !IsBlupiHereEx(GetCel(pos.x+0,pos.y+0),
							GetCel(pos.x+1,pos.y+1), -1, FALSE) )
		{
			if ( m_decor[pos.x/2][pos.y/2].floorChannel == CHFLOOR &&
				 m_decor[pos.x/2][pos.y/2].floorIcon >=  2 &&
				 m_decor[pos.x/2][pos.y/2].floorIcon <= 14 )  // rive ou eau ?
			{
				return FALSE;
			}

			return TRUE;
		}
	}

	return FALSE;
}

// Modifie le d�cor lors d'un remplissage.

void CDecor::ArrangeFillPut(POINT pos, int channel, int icon)
{
	if ( m_bFillFloor )
	{
		PutFloor(pos, channel, icon);
		ArrangeFloor(pos);
	}
	else
	{
		if ( icon >= 0 && icon <= 5 )  // plantes ?
		{
			icon = Random(0,5);
		}
		if ( icon >= 6 && icon <= 11 )  // arbres ?
		{
			icon = Random(6,11);
		}
		if ( icon >= 37 && icon <= 43 )  // rochers ?
		{
			icon = Random(37,43);
		}
		PutObject(pos, channel, icon);
		ArrangeObject(pos);
	}
}

// Rempli un sol � partir d'une position donn�e.

void CDecor::ArrangeFillSearch(POINT pos)
{
	int		startX, endX;

	// Cherche la borne gauche.
	startX = pos.x;
	endX   = pos.x;
	while ( pos.x > 0 && ArrangeFillTest(pos) )
	{
		pos.x -= 2;
	}
	startX = pos.x+2;

	// Cherche la borne droite.
	pos.x = endX;
	while ( pos.x < MAXCELX-2 && ArrangeFillTest(pos) )
	{
		pos.x += 2;
	}
	endX = pos.x-2;

	// Rempli toute la ligne trouv�e.
	pos.x = startX;
	while ( pos.x <= endX )
	{
		m_pFillMap[(pos.x/2)+(pos.y/2)*(MAXCELX/2)] = 1;
		pos.x += 2;
	}

	// Cherche la ligne au-dessus.
	if ( pos.y > 0 )
	{
		pos.y -= 2;
		pos.x = startX;
		while ( pos.x <= endX )
		{
			while ( pos.x <= endX && !ArrangeFillTest(pos) )
			{
				pos.x += 2;
			}
			if ( pos.x > endX )  break;

			if ( ArrangeFillTest(pos) )
			{
				ArrangeFillSearch(pos);  // appel r�cursif
			}

			while ( pos.x <= endX && ArrangeFillTest(pos) )
			{
				pos.x += 2;
			}
		}
		pos.y += 2;
	}

	// Cherche la ligne au-dessous.
	if ( pos.y < MAXCELY-2 )
	{
		pos.y += 2;
		pos.x = startX;
		while ( pos.x <= endX )
		{
			while ( pos.x <= endX && !ArrangeFillTest(pos) )
			{
				pos.x += 2;
			}
			if ( pos.x > endX )  break;

			if ( ArrangeFillTest(pos) )
			{
				ArrangeFillSearch(pos);  // appel r�cursif
			}

			while ( pos.x <= endX && ArrangeFillTest(pos) )
			{
				pos.x += 2;
			}
		}
	}
}

// Rempli un sol � partir d'une position donn�e.

void CDecor::ArrangeFill(POINT pos, int channel, int icon, BOOL bFloor)
{
	m_bFillFloor = bFloor;

	pos.x = (pos.x/2)*2;
	pos.y = (pos.y/2)*2;

	m_fillPutChannel = channel;
	m_fillPutIcon    = icon;

	if ( bFloor )
	{
		GetFloor(pos, m_fillSearchChannel, m_fillSearchIcon);
	}
	else
	{
		GetObject(pos, m_fillSearchChannel, m_fillSearchIcon);
	}

	m_pFillMap = (char*)malloc(MAXCELX*MAXCELY*sizeof(char)/4);
	if ( m_pFillMap == NULL )  return;
	memset(m_pFillMap, 0, MAXCELX*MAXCELY*sizeof(char)/4);

	ArrangeFillSearch(pos);

	for ( pos.x=0 ; pos.x<MAXCELX ; pos.x+=2 )
	{
		for ( pos.y=0 ; pos.y<MAXCELY ; pos.y+=2 )
		{
			if ( m_pFillMap[(pos.x/2)+(pos.y/2)*(MAXCELX/2)] == 1 )
			{
				ArrangeFillPut(pos, channel, icon);
			}
		}
	}

	free(m_pFillMap);
}


// Supprime tous les personnages bloqu�s dans des murs
// ou debout sur l'eau.

void CDecor::ArrangeBlupi()
{
	int		rank;

	for ( rank=0 ; rank<MAXBLUPI ; rank++ )
	{
		if ( m_blupi[rank].bExist )
		{
			if ( !IsFreeCel(m_blupi[rank].cel, rank) )
			{
				m_blupi[rank].bExist = FALSE;
			}
		}
	}
}

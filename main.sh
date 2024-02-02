#!/bin/bash

#Création du chrono pour calculer le temps d'exécution
start_chrono(){
	start_time=$(date +%s)
	echo "Chronomètre démarré"
}



stop_chrono(){
	end_time=$(date +%s)
	elapsed_time=$((end_time - start_time))
	echo "\nChronomètre arrêté."
	echo "Temps écoulé : $elapsed_time secondes.\n"
}


#Création des dossiers 
directories(){
	file="temp"
	if [ -d "$file" ]
	then 
		find temp -mindepth 1 -delete
	fi
	
	#permet de créer les dossiers images et temp
	mkdir -p temp images
}


directories

#On traite maintenant les différentes options du projet pas besoin de help on a tous les traitements ici
function(){
	echo "Veuillez choisir une option"
	echo "d1 - Les 10 conducteurs avec le plus de trajets"
	echo "d2 - Les 10 conducteurs ayant parcouru la plus grande distance"
	echo "l - Les 10 trajets les plus longs"
	echo "t - Les 10 villes les plus traversées"
	echo "s - Les statistiques sur les étapes"
	
	read -p "Votre choix : " choix
	case $choix in
		d1)
			start_chrono
			#Trie le nombre de trajets pour chaque conducteur d1
			awk -F";" '/;1;/ {compteur[$6] +=1} END {for (nom in compteur) print compteur[nom] ";" nom}' data/data.csv | sort -nrk1,1 | head -n 10 > temp/temp_d1.csv
			
			stop_chrono
			#Affiche les 10 conducteurs avec le plus de trajets
			cat temp/temp_d1.csv
			
			#Gnuplot pour faire le graphique de d1
			gnuplot <<EOF
			
			set size square 1,1.1
			set term png size 600,900

			set datafile separator ";"
			set style fill solid 
			set boxwidth 1.5 
			set xlabel "Conducteurs" rotate by 90 offset -30,2
			set x2label "Nombre de trajets" rotate by 90 offset 28,2 
			set ylabel "Conducteurs avec le plus de trajets" font '0,15' offset 2,0
			set xtic rotate by 90 font '0,10' offset 0,-9.5
			set ytics rotate by 90 font '0,11' offset 59,1
			set style data histograms
			set output 'temp/temp_d1.png'
			plot 'temp/temp_d1.csv' using 1:xticlabels(2) notitle lc rgb "light-salmon"
EOF
			convert "temp/temp_d1.png" -rotate 90 "images/data_d1"
			rm temp/temp_d1.png
			rm temp/temp_d1.csv
			#Affiche le graphique
			display images/data_d1

			
			;;
			
		d2)
			start_chrono
			#Compteur de la plus grande distance 

			awk -F";" '{compteur[$6] += $5} END {for (nom in compteur) print nom ";" compteur[nom]}' data/data.csv | sort -t";" -k2,2nr | head -n 10 > temp/temp_d2.csv
			stop_chrono
			
			#Affiche les 10 conducteurs ayant parcouru la plus grande distance
			cat temp/temp_d2.csv

			#Création du graphique de d2
			gnuplot <<EOF
			
			set size square 1,1.1
			set term png size 600,800

			set datafile separator ";"
			set style fill solid 
			set boxwidth 1.5 
			set xlabel "Conducteurs" rotate by 90 offset -30,2
			set x2label "km parcourus" rotate by 90 offset 28,2 
			set ylabel "Conducteurs ayant la plus grande distance" font '0,15' offset 2,0
			set xtic rotate by 90 font '0,10' offset 0,-9.5
			set ytics rotate by 90 font '0,11' offset 59,1
			set style data histograms
			set output 'temp/temp_d2.png'
			plot 'temp/temp_d2.csv' using 2:xticlabels(1) notitle lc rgb "slategrey"
EOF
			convert "temp/temp_d2.png" -rotate 90 "images/data_d2"
			rm temp/temp_d2.png
			rm temp/temp_d2.csv
			
			#Affiche le graphique 
			display images/data_d2
			;;
			
		l)
			start_chrono
			#Compteur des 10 trajets les plus long l
			awk -F";" '{compteur[$1] += $5} END {for (id_trajet in compteur) print id_trajet ";" compteur[id_trajet]}' data/data.csv | sort -t";" -k2nr | head -n 10 | sort -t";" -k1,1nr > temp/temp_l.csv
			stop_chrono
			
			#Affiche Les 10 trajets les plus longs
			cat temp/temp_l.csv
			
			#Création du graphique de l
			gnuplot <<EOF 
			set size square 1,1.1
			set term png size 600,800

			set datafile separator ";"
			set style fill solid 
			set boxwidth 1.5 

			set title "Les 10 trajets les plus longs" 
			set xlabel "ID route"  
			set ylabel "Distance (en km)" 
			set xtics rotate by -45

			set style data histograms

			set output 'temp/temp_l.png'
			plot 'temp/temp_l.csv' using 2:xticlabels(1) notitle lc rgb "light-turquoise"
EOF
			convert "temp/temp_l.png" "images/data_l"
			rm temp/temp_l.png
			rm temp/temp_l.csv
			
			#Affiche les 10 trajets les plus longs
			display images/data_l
			
			;;
		t)
			start_chrono
			
			#Trie les données pour pouvoir utiliser le c 
			awk -F";" 'NR > 1 {tab[$1";"$4] +=1; if ($2==1) {tab[$1";"$3]+=1; deb[$1";"$3]=1}} END {for (ville in tab) print ville ";" tab[ville] ";" deb[ville] }' data/data.csv | awk -F";" '{tab[$2]+=1; deb[$2]+=$4} END {for (ville in tab) print ville "," tab[ville] "," deb[ville]}' > temp/t_tri.csv 

			#Compile directement dans le shell pas besoin de Makefile
			gcc -o progc/t progc/t.c
			./progc/t  temp/t_tri.csv

			sort -t';' -k1,1 temp/fin.csv > temp/t.csv
	
			stop_chrono
			#Affiche Les 10 villes les plus traversées
			cat temp/t.csv
			
			
			#Création du graphique de t
			gnuplot <<EOF
			set terminal png 
			set output "images/data_t"

			set style data histograms 
			set style fill solid 1.00 border -1
			set xlabel "Nom des villes "  
			set ylabel "Nb routes " 
			# Définition des couleurs pour les colonnes
			set style fill solid noborder
			set boxwidth 0.5
			set style histogram cluster gap 1

			# Définition des couleurs

			# Titre du graphique
			set title "Les 10 villes les plus traversées"

			# Rotation des étiquettes x
			set xtics rotate by -80

			# Séparateur de données
			set datafile separator ","

			# Affichage du graphique
			plot 'temp/t.csv' using 2:xtic(1) title "Total route",'' using 3 title "Première ville"
EOF
			#Affiche le graphique de t 
			display images/data_t
			rm temp/fin.csv
			;;
		s)
			start_chrono
			
			#trie les données pour pouvoir utiliser le c 
            		cut -d';' -f1,2,5 data/data.csv > temp/s1.csv 
            		tail -n +2 temp/s1.csv > temp/s2.csv
            		
            		
            		#Compile directement dans le shell pour pas utiliser de MakeFile
            		gcc -o progc/s progc/s.c
           		./progc/s temp/s2.csv


            		head -n 50 temp/sortie.csv > temp/s.csv
            		
            		stop_chrono

			#Affiche les statistiques sur les étapes
            		cat temp/s.csv
            		
            		gnuplot << EOF 
            		set terminal png size 900,600
			set output 'images/data_s'

			set datafile separator ";"

			set title "Statistiques sur les étapes"
			set border 4095 front lt black linewidth 1.000 dashtype solid 

			set xlabel 'Route ID' 
			set ylabel 'Distance (km)' 

			set xtics rotate by -80 
			

			Shadecolor = "light-green"


			plot 'temp/s.csv' using 0:2:3:xticlabels(1) with filledcurve fc rgb Shadecolor title "Distance Min/Max", \
  '' using 0:4:xticlabels(1) smooth mcspline lw 2 title "Distance average"
            
EOF
            		rm temp/sortie.csv temp/s1.csv temp/s2.csv 
            		
            		#Affiche le graphique de s 
            		display images/data_s
            		
			;;
			
		*)
			echo "L'option $choix n'existe pas. Veuillez réessayer;"
			exit 1
			;;
	esac
}


function
			
				
			
			

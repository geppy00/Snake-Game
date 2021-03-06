//REPLY SNAKE GAME DEVELOPED IN C
//DEVELOPER/CODER: Giuseppe Malafronte

//PER ESEGUIRE IL PROGRAMMA CON IL COMPILATORE gcc O g++ BISOGNA IMPORTARE IL LINKER MANUALMENTE CON IL COMANDO -l SCRIVENDO NELLA SHELL -lWinmm

#include<windows.h>
#include<mmsystem.h>
#include <conio.h>
#include <ctype.h>
#include<stdio.h>
#include<Windows.h>
#include<stdlib.h>
#include<time.h>

#pragma comment(lib, "Winmm.lib")

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996)

#define N 20 //NUMERO DI RIGHE (Altezza del frame)
#define M 110 //NUMERO DI COLONNE (Base del frame)

/*Dichiarazioni di variabili globali dove*/
int i;	//'i' e 'j' servirano per i vari loop
int j; 
int field[N][M];	//la matrice field sarà il contenitore delle posizioni nel campo di gioco
int x;	//'x' e 'y' saranno le coordinate del nostro serpente
int y;
int Gy;		//la variabile Gy serve per mantenere il valore della coordinata y
int head;	//head e tail saranno le coordinate rispettivamente per la testa e la coda ed ogni volta che la matrice contiene il valore di head quella è la testa ed ogni volta che contiene il valore di tail quello sarà sempre il serpente, altri valori tra tail e head sarà il corpo del serpente quindi possiamo effettivamente ottenere un serpente nel nostro field[][]
int tail; 
int game;	//game è una variabile usata per il game loop 
int food;	//food è una variabile che conterrà la coordinata del cibo del serpente
int xFood;	//xFood E yFood sono le coordinate della posizione del cibo
int yFood; 
int comandoInput;	//comandoInput conterrà il valore ascii del tasto premuto ed inizializzato a d cosi che gia si muova il serpente
int direction;	//direction indica la direzione del serpente ed è iniziallizata a d cioe verso destra 
int score;	//score è un contatore che farà visualizzare il punteggio al momento della partita 
int highScore;	//highScore conterrà il punteggio massimo 
int speed;	//speed servirà per modificare la velocita del serpente
int difficolta;
FILE* f;	//*f sarà il puntatore al file dove sarà contenuto il punteggio massimmo raggiunto

void snakeInitialization(void) {	//Questa funzione viene chiamata ogni volta che inizia il gioco, il suo compito è quello di settare tutti i valori default per il nostro serpente
	f = fopen("highScore.txt", "r");
	if (f == NULL) {
		printf("ERRORE LETTURA DEL FILE highScore.txt\n\n");
		exit(1);
	}
	int valFscanf = fscanf(f, "%d", &highScore);
	fclose(f);

	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++)
			field[i][j] = 0;
	}

	x = N / 2;
	y = M / 2;
	head = 5;
	tail = 1;
	Gy = y;
	game = 0; //0 significa che il gioco è in esecuzione(1 invece vul dire che il gioco è terminato)
	food = 0;
	score = 0;
	speed = 99;
	direction = 'd';

	for (i = 0; i < head; i++) {
		Gy++;
		field[x][Gy - head] = i + 1;
	}
}

void printTitleGame() {
	printf("\n");
	printf("\t\t\t+-+-+-+-+-+");
	printf("| S | N | A | K | E |");
	printf("| G | A | M | E | ! |");
	printf("+-+-+-+-+-+");
	printf("\n");
}

void print(void) {
	printf("    CURRENT SCORE: %d\tHIGHSCORE: %d", score, highScore);
	printf("\n");
	//Creazione del frame(area dove il serpente può muoversi) attraverso i caratteri ASCII
	for (i = 0; i <= M + 1; i++) {
		if (i == 0)
			printf("%c", 201);
		else if (i == M + 1)
			printf("%c", 187);
		else
			printf("%c", 205);
	}

	printf("\n");

	//In questo for oltre a disegnare una perte di frame disegniamo anche il serpente, e il cibo in base ai volori contenuti nella matrice
	for (i = 0; i < N; i++) {
		printf("%c", 186);
		for (j = 0; j < M; j++) {
			if (field[i][j] == 0)
				printf(" ");
			if (field[i][j] > 0 && field[i][j] != head)
				printf("%c", 176);
			if (field[i][j] == head)
				printf("%c", 178);
			if (field[i][j] == -1)
				printf("%c", 15);
			if (j == M - 1) 
				printf("%c\n", 186);
		}
	}

	for (i = 0; i <= M + 1; i++) {
		if (i == 0)
			printf("%c", 200);
		else if (i == M + 1)
			printf("%c", 188);
		else
			printf("%c", 205);
	}

}

void resetScreenPosition(void) {
	HANDLE hOut;	//serve per gestire vari problemi della console del nostro programma
	COORD postion;	//mantine le post-coordinate sullo schermo della console 
	hOut = GetStdHandle(STD_OUTPUT_HANDLE); //abbiamo assegnato alla variaviale hOut lo Standard Input del nostro programma per risolvere certi problemi come l'handling
	postion.X = 0;
	postion.Y = 0; //Abbiamo assegnato angolo(nel nostro acso a sinistra) più in alto dell'output schermo alle variabili "position"
	SetConsoleCursorPosition(hOut, postion); //qui abbiamo detto la posizione della console a quella "posizione"
}

void randomFood(void) {
	srand(time(0));		//serve per rendere la posizione del cibo imprevedibile
	xFood = 1 + rand() % 18;
	yFood = 1 + rand() % 38;	//genera un numero a caso e quello sarà la coordinata del cibo e grazie a srand queste coordinate combieranno sempre ad ogni esecuzione

	if (food == 0 && field[xFood][yFood] == 0) {	//questo if serve per controllare se posso stampare il cibo sullo schermo, per farlo si devono verificare due condizioni la prima e che non ci sia altro cibo sullo schermo e la seconda è che se il campo di gioco è vuoto(vuol dire che in quelle coordinate non ce il serpente) alora posso stampare
		field[xFood][yFood] = -1;	//valore speciale ustato nella funzione print per stampare il cibo
		food = 1;	//serve ad indicare che ce gia un cibo sul campo di gioco
	}

	if (speed > 10 && score != 0)
		speed -= 5;
}

int getCharacterWithNoBlock(void) {
	if (_kbhit())	//_kbhit restituisce un valore diverso da zero se è stato premuto un tasto. In caso contrario, viene restituito 0.
		return _getch();	//La funzione _getch legge un singolo carattere dalla console senza echeggiare il carattere
	else
		return -1; //se al momento della chiamata non viene premuto nessun tasto allora restituiamo -1 

}


void gameOver(void) {		//questa funzione emette un suono e cambia il valore di game ad 1 che vuol dire che il gioco è terminato
	PlaySoundA((LPCSTR) "C:\\Users\\peppe\\Desktop\\ALL MY SHIT\\Progetti in C\\(games) SNAKE\\Mario_Death.wav", NULL, SND_FILENAME | SND_ASYNC);	// interrompe la riproduzione della traccia di gioco
	//printf("\a");
	Sleep(1500);
	system("Cls");
	
	if (score > highScore) {
		printf("!!!!!!!!!!!!!!! NEW HIGHSCORE %d !!!!!!!!!!!!!!!\n\n", score);
		system("pause");
		f = fopen("highScore.txt", "w");
		fprintf(f, "%d", score);
		fclose(f);
	}

	system("Cls");
	printf("\n\n\t\t\t\t!!!!!!!!!!!!!!!!! GAME OVER !!!!!!!!!!!!!!!!!\n\n");
	printf("Punteggio totalizzato => SCORE: %d\n\n", score);

	printf("Premi invio per giocare di nuovo\nOppure ESC per uscire\n\n");
	while (1) {
		comandoInput = getCharacterWithNoBlock();
		if (comandoInput == 13) {
			game = 0;
			snakeInitialization();
			PlaySoundA((LPCSTR) "C:\\Users\\peppe\\Desktop\\ALL MY SHIT\\Progetti in C\\(games) SNAKE\\track.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			break;
		}
		else if (comandoInput == 27) {
			game = 1;
			break;
		}
	}

	system("cls");
}

void movment(void) {
	comandoInput = getCharacterWithNoBlock();	//prendo il valore ascii del tasto premuto
	comandoInput = tolower(comandoInput);	//usiamo la funzione tolower in modo da rendere il comando unico e funzionerà anche se l'utente attiva la maiuscola

	if ((comandoInput == 'a' || comandoInput == 'd' || comandoInput == 'w' || comandoInput == 's' || comandoInput == 27) && abs(direction - comandoInput) > 5)
		direction = comandoInput;	//questo serve per far si che il serpente si muova all'infito premendo solo una volta il tasto

	if (direction == 'd') {
		y++;
		if (field[x][y] != 0 && field[x][y] != -1)
			gameOver();
		if (y == M - 1)
			y = 0;
		if (field[x][y] == -1) {
			food = 0;
			score += 5;
			tail -= 2;
		}
		head++;
		field[x][y] = head;
	}
	//facciamo un a serie di controlli in base al tasto premuto il serpente combierà posizione e quindi anche le sue coordinate quindi bisogna aggiornarle
	if (direction == 'a') {
		y--;
		if (field[x][y] != 0 && field[x][y] != -1)
			gameOver();
		if (y == 0)
			y = M - 1;
		if (field[x][y] == -1) {
			food = 0;
			score += 5;
			tail -= 2;
		}
		head++;
		field[x][y] = head;
	}

	if (direction == 'w') {
		x--;
		if (field[x][y] != 0 && field[x][y] != -1)
			gameOver();
		if (x == 0)			//in ogni direzione bisogna controllare se il serpente va in un bordo e dobbiamo spostarlo alla parte opposta altrimenti i valori di x(in caso vada sopra) o y(in caso vada a destra) andranno msotto lo zero valori non contenuti nella matrice field e il gioco crasherà
			x = N - 1;
		if (field[x][y] == -1) {		//quest if serve ad controllare se il cibo è stato mangiato con il valore -1 significa che nel campo di gioco è presente il cibo quando il serpente ci va sopra aggiorniamo il valore food a 0 cosi la funzione randomFood generarà un altro cibo sulla mappa poi quando mangiamo il cibo allunghiamo il serpende decrementando la coda
			food = 0;
			score += 5;
			tail -= 2;
		}
		head++;
		field[x][y] = head;
	}

	if (direction == 's') {
		x++;
		if (field[x][y] != 0 && field[x][y] != -1)  //Quest if controlla se il serpente vada su se stesso e le condizioni sono due se non si trova in uno spazio vuoto e se non si strova in uno spazio dove ce il cibo, quindi se si verificano tutte e due allora sta su se stesso e ci chiamiamo la funzione game over per terminare il gioco
			gameOver();
		if (x == N)
			x = 0;
		if (field[x][y] == -1) {
			food = 0;
			score += 5;
			tail -= 2;
		}
		head++;
		field[x][y] = head;
	}

	if (direction == 27) 
		gameOver();
	
}


void tailRemove(void) {
	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			if (field[i][j] == tail)
				field[i][j] = 0;		//questa funzione serve per rimuovere la coda in eccesso e aggiornare la posizione
		}
	}

	tail++;
}

void endTitle() {
	printf("\n\t\t\t\t*****************SNAKE C VERSION*****************\n");
	printf("\nGrazie per aver giocato :D\n");
	printf("\n\tCREDITS:\n\n\t\tDeveloper = Giuseppe Malafronte (G E P P Y)\n\t\tTester = Giovanni (Crax)\n\n\t\tUn ringraziamento speciale a: Daniil Khalikov e Jeft Faria Mathamba\n\n\n");
	system("pause");
}

void menu() {
	printf("\n======================================================================================================================\n\n");
	printf("\nSELEZIONARE UN TASTO NUMERICO DEL MENU' PER ESEGUIRE UNA DETERMINATA OPERAZIONE\n");
	puts("\t\tWELCOME TO SNAKE ");
	puts("\t\tPRESS 1 TO START PLAY THE GAME");
	puts("\t\tPRESS 2 TO RESERT HIGHSCORE");
	puts("\t\tPRESS 0 TO EXIT");
	printf("\n\n====================================================================================================================\n");
}

int controlloSceltaDifficolta() {
	int sceltaDif;
	
	do {
		printf("\n======================================================================================================================\n\n");
		printf("\nSCEGLI LA DIFFICOLTA'\n");
		puts("\t\t1 FACILE");
		puts("\t\t2 NORMALE");
		puts("\t\t3 DIFFICILE");
		printf("\n\n====================================================================================================================\n");
		fflush(stdin);

		if(scanf("%d", &sceltaDif) > 0 && (sceltaDif == 1 || sceltaDif == 2 || sceltaDif == 3)) {
			PlaySoundA((LPCSTR) "C:\\Users\\peppe\\Desktop\\ALL MY SHIT\\Progetti in C\\(games) SNAKE\\Select_Menu.wav", NULL, SND_FILENAME | SND_ASYNC);
			return sceltaDif;
		}
		else {
			PlaySoundA((LPCSTR) "C:\\Users\\peppe\\Desktop\\ALL MY SHIT\\Progetti in C\\(games) SNAKE\\Nope.wav", NULL, SND_FILENAME | SND_ASYNC);
			system("cls");
			printf("\n!!\tSCEGLI UN OPZIONE PRESENTE NEL MENU'\t!!\n");
		}
	}while(sceltaDif < 1 || sceltaDif > 3);
}

void menuScelta() {
	int scelta;
	int sceltaDifficolta;
	
	do {
		menu();
		if(scanf("%d", &scelta) > 0 && (scelta == 1 || scelta == 2 || scelta == 0))
			PlaySoundA((LPCSTR) "C:\\Users\\peppe\\Desktop\\ALL MY SHIT\\Progetti in C\\(games) SNAKE\\Select_Menu.wav", NULL, SND_FILENAME | SND_ASYNC);

		switch (scelta) {
			case 1:
				system("cls");

				sceltaDifficolta = controlloSceltaDifficolta();
				
				if(sceltaDifficolta == 1)
					difficolta = 99;
				if(sceltaDifficolta == 2)
					difficolta = 33;
				if(sceltaDifficolta == 3)
					difficolta = 0;

				return;
			break;

			case 2:
				fflush(stdin);
				f = fopen("highScore.txt", "w");
				fprintf(f, "0");

				system("cls");
				sceltaDifficolta = controlloSceltaDifficolta();
				
				if(sceltaDifficolta == 1)
					difficolta = 99;
				if(sceltaDifficolta == 2)
					difficolta = 33;
				if(sceltaDifficolta == 3)
					difficolta = 0;

				return;
			break;

			case 0:
				system("cls");
				endTitle();
				system("color 0F");
				exit(0);
				break;

			default:
				PlaySoundA((LPCSTR) "C:\\Users\\peppe\\Desktop\\ALL MY SHIT\\Progetti in C\\(games) SNAKE\\Nope.wav", NULL, SND_FILENAME | SND_ASYNC);
				system("cls");
				puts("\n\t\t\t\t!! IL COMANDO NON E' PRESENTE NEL MENU' !!");
				puts("\n\t\t\t\tPER FAVORE SELEZIONARE UN COMANDO PRESENTE NEL MENU'\n\n");
		}
	} while (scelta != 0);
}

void main(void) {
	system("cls");
	system("color 6E");
	menuScelta();
	system("cls");
	snakeInitialization();
	PlaySoundA((LPCSTR) "C:\\Users\\peppe\\Desktop\\ALL MY SHIT\\Progetti in C\\(games) SNAKE\\track.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);	//riproduce un file audio ed utilizza un identificatore per l'evento di sistema (LPCSTR)

	while (game == 0) {
		printTitleGame();
		printf("\n");
		print();	//stampa tutto quello che ce nel gioco
		printf("\n");
		printTitleGame();
		resetScreenPosition();	//Resetta lo screen position portando il cursore all'anglolo più in alto a sinistra, sarà costante farà si trova nello stesso posto senza fermare l'esecuzione
		randomFood();	//genera le coordinate per il cibo 
		movment();	//funzione che controlla quale movimento deve effettuare il serpente
		tailRemove();	//funzione che rimuove la coda in eccesso quando si muove il serpente
		Sleep(difficolta);	//funzione per fa si che il gioco (principalmente i movimenti del serpente) siano fluidi
	}

	endTitle();
	system("color 0F");
}
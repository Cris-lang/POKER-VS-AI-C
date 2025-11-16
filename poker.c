#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
    #define SLEEP(seconds) Sleep((seconds) * 1000)
#else
    #include <unistd.h>
    #define SLEEP(seconds) sleep(seconds)
#endif

#define clrscr() printf("\e[1;1H\e[2J")		//clear terminal

#define MAX_CARD 5	//card for each hand
#define DECK_CARD 52

char *values[13] = {"A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};
char *suits[4] = {"Hearts", "Diamonds", "Clubs", "Spades"};

typedef enum{
	High_card = 1,
	Pair = 2,
	Two_pair = 4,
	Three_of_a_kind = 8,
	Straight = 16,
	Flush = 32,
	Full_house = 64,
	Four_of_a_kind = 128,
	Royale_flush = 256,
}score;

struct card{
	char *value;
	char *suit;
};

struct card deck[DECK_CARD];
int top_card = DECK_CARD -1;	//index of first possible card to draw

struct card player_hand[MAX_CARD], ai_hand[MAX_CARD];
struct card ai_cards[2], pg_cards[2], table_cards[3]; 	//helps understand who the cards belong to
int player_fiches = 1500, ai_fiches = 1500;

void create_deck();
void shuffle_deck();
void draw_cards();
void ordinate_hands();
void print_player_cards();
void print_table_cards(int how_many_card);
void print_result(score player_score, score ai_score, int plate_fiches);

int get_value_from_index(char *value);

score check_score(struct card hand[], int is_player);

char *score_to_string(score s);


int main(void){
	srand(time(NULL));
    create_deck();

    char input;
    int plate_fiches;

    printf("\nWelcome to this poker table, sir.\n");
    printf("\nYou and the AI will start with %d fiches...\nFirst to lose all the fiches loses the game.\n", player_fiches);
    printf("\nThe game is simple, both of you will draw two cards...\nAnd i will reveal one of the three table cards.");
    printf("\nThen you can decide if you want to bet fiches or fold...\nIf both decide to continue i will reveal another card, and so on..."); 
    printf("\nAfter three cards the player with the better hand wins the round and the fiches!\nBut if it's a draw the house wins!\n");
    printf("\nPress any to start.\n");
    getchar();

    clrscr();
    printf("\nThe dealer is shuffling the deck...\n");
    SLEEP(3);
   
    do{	
        plate_fiches = 0;	//set the table fisches to zero each round

        //if the deck has fewer than 10 card we need to create a new one
        if(top_card < 7){
            printf("\nDeck too small, reshuffling...\n");
            create_deck();
            top_card = DECK_CARD - 1;
            SLEEP(3);
        }
   
        clrscr();
        printf("\nThe dealer is dealing the cards...\n");
        SLEEP(3);
        //shuffle the array of cards
        shuffle_deck();
        //draw table and players cards
        draw_cards();
        ordinate_hands();
 
        //check the best score for each hand
        score player_score = check_score(player_hand, 1);
        score ai_score = check_score(ai_hand, 0);
 
       	//print the 2 player cards and the first table card	
       	printf("\nThe dealer is revealing the first table card:");
       	printf("\nPlayer Cards: ");
        print_player_cards();
        printf("\nTable Cards: ");
        print_table_cards(1);
        printf("\n\nDo you want to bet 100 fisches or fold (y/n) :");
    	scanf(" %c", &input);
       	if(input == 'n'){
       		goto ai_wins;
       	}else{
       		if(player_fiches != 0){
       			player_fiches -= 100;
       			plate_fiches += 100;
       		}else{
       			printf("Already all in!\n");
       		}

       		//ia random move
       		int quality_of_cards = ai_score;
       		int decision = quality_of_cards + ((rand() % 100) - 30);
       		if(decision < 50){
       			if(ai_fiches != 0){
       				ai_fiches -= 100;
       				plate_fiches += 100; 
       			}else{
       				printf("AI already all in!\n");
       			}
       		} else{
       			goto player_wins;
       		}
       	}
    
       	//print the 2 player cards and 2 table cards
        printf("\n\n\nThe dealer is revealing the second table card: ");
        printf("\nPlayer Cards: ");
        print_player_cards();
        printf("\nTable Cards: ");
        print_table_cards(2);
        printf("\n\nDo you want to bet 100 fisches or fold (y/n) :");
    	scanf(" %c", &input);
       	if(input == 'n'){
       		goto ai_wins;
       	}else{
       		if(player_fiches != 0){
       			player_fiches -= 100;
       			plate_fiches += 100;
       		}else{
       			printf("Already all in!\n");
       		}

       		//ia random move
       		int quality_of_cards = ai_score;
       		int decision = quality_of_cards + ((rand() % 100) - 30);
       		if(decision < 50){
       			if(ai_fiches != 0){
       				ai_fiches -= 100;
       				plate_fiches += 100; 
       			}else{
       				printf("AI already all in!\n");
       			} 
       		} else{
       			goto player_wins;
       		}
       	}
     
       	//print the 2 player cards and 3 table cards
        printf("\n\n\nThe dealer is revealing the last table card: ");
        printf("\nPlayer Cards: ");
        print_player_cards();
        printf("\nTable Cards: ");
        print_table_cards(3);
        //show result and ia cards
        print_result(player_score, ai_score, plate_fiches);
        goto round_end;

        ai_wins: 
        printf("\nPlayer leaves the table, AI wins the round and %d fiches\n", plate_fiches);
        ai_fiches += plate_fiches;
        goto round_end;

        player_wins:
        printf("\nAI leaves the table, Player wins the round and %d fiches\n", plate_fiches);
        player_fiches += plate_fiches;

        round_end:
        if(player_fiches == 0 && ai_fiches == 0){
    		printf("\nBoth players have lost their fiches! Nobody can beat the house int the end *hah-hah-hah*.\n");
    	}else if(ai_fiches == 0){
    		printf("\nPLAYER WINS! AI has lost all its fiches.\n");
   		}else if(player_fiches == 0){
    		printf("\nAI WINS! Player has lost all its fiches.\n");
    	}else{
    		printf("\nPlayer fisches : %d,  AI fisches: %d\nKeep playing? (y/n): ", player_fiches, ai_fiches);
        	scanf(" %c", &input);
   	 	}
    }while((input == 'y' || input == 'Y') && player_fiches > 0 && ai_fiches > 0);

    return 0;
}

//create a deck of 52 cards
void create_deck(){
	int offset = 0;
	for(int i = 0; i < 4; i++){
		for(int j = 0; j < 13; j++){
			deck[offset].value = values[j];
			deck[offset].suit = suits[i];
			offset++;
		}
	}
}

//shuffle the deck of 52 cards in random order
void shuffle_deck(){
	for(int i = DECK_CARD -1; i > 0; i--){
		int random_pos = rand() % (i + 1);
		struct card temp = deck[i];

		deck[i] = deck[random_pos];
		deck[random_pos] = temp;
	}
}

//pick 7 cards each round, 2 for player, 2 for ia and 3 for table
void draw_cards(){
	//pick 2 cards for the player
	for(int i = 0; i < 2; i++){
		player_hand[i].value = deck[top_card].value;
		pg_cards[i].value = deck[top_card].value;
		pg_cards[i].suit = deck[top_card].suit;
		player_hand[i].suit = deck[top_card--].suit;
	}
	//pick 2 cards for the ia
	for(int i = 0; i < 2; i++){
		ai_hand[i].value = deck[top_card].value;
		ai_cards[i].value = deck[top_card].value;
		ai_cards[i].suit = deck[top_card].suit;
		ai_hand[i].suit = deck[top_card--].suit;
	}
	//copy the 5 table card in each hand
	for(int i = 2; i < MAX_CARD; i++){
		table_cards[i].value = deck[top_card].value;
		table_cards[i].suit = deck[top_card].suit;
		player_hand[i].value = deck[top_card].value;
		player_hand[i].suit = deck[top_card].suit;
		ai_hand[i].value = deck[top_card].value;
		ai_hand[i].suit = deck[top_card--].suit;
	}
}

//convert string value of a card to int
int get_value_from_index(char *value){
    if(strcmp(value, "A") == 0) return 1;
    if(strcmp(value, "J") == 0) return 11;
    if(strcmp(value, "Q") == 0) return 12;
    if(strcmp(value, "K") == 0) return 13;
    return atoi(value); //for values between 2 and 10 we can use atoi 
}

//sort the hands in descending order
void ordinate_hands(){
	//player hand
	for(int i = 0; i < MAX_CARD -1 ; i++){
		for(int j = i + 1; j < MAX_CARD; j++){
			if(get_value_from_index(player_hand[i].value) < get_value_from_index(player_hand[j].value)){
				struct card temp = player_hand[i];
				player_hand[i] = player_hand[j];
				player_hand[j] = temp;
			}
		}	
	}
	//ai hand
	for(int i = 0; i < MAX_CARD -1 ; i++){
		for(int j = i + 1; j < MAX_CARD; j++){
			if(get_value_from_index(ai_hand[i].value) < get_value_from_index(ai_hand[j].value)){
				struct card temp = ai_hand[i];
				ai_hand[i] = ai_hand[j];
				ai_hand[j] = temp;
			}
		}	
	}
}

score check_score(struct card hand[], int is_player){	//is_player helps to know where we need to store the highest value in hand
	//check the frequency of each card value
	int counts[14] = {0};

	for(int i = 0; i < MAX_CARD; i++){
        counts[get_value_from_index(hand[i].value)]++;
    }
 	
 	//check if there are pairs, three_of_a_kind or Four_of_a_kind 
 	int pair = 0, tris = 0, four = 0;
 	for(int i = 1; i <= 13; i++){
 		if(counts[i] == 2){
 			pair++;
 		}
 		if(counts[i] == 3){
 			tris++;
 		}
 		if(counts[i] == 4){
 			four++;
 		}
 	}

	//check royale flush
	int flag = 1;
	for(int i = 0; i < MAX_CARD-1; i++){
		if(strcmp(hand[i].suit, hand[i+1].suit) == 0){
			if((get_value_from_index(hand[i].value) != (get_value_from_index(hand[i+1].value)) + 1)){
				flag = 0;
			}
			if((get_value_from_index(hand[i].value) == 10) && (get_value_from_index(hand[i+1].value) == 1)){
				flag = 1;
			}
		}else{
			flag = 0;
		}
	}
	if(flag) return Royale_flush;

	//check four of a kind
	if(four) return Four_of_a_kind;

	//check full house
	if(pair && tris) return Full_house;

	//check flush
	flag = 1;
	for(int i = 0; i < MAX_CARD-1; i++){
		if(strcmp(hand[i].suit, hand[i+1].suit) != 0){
			flag = 0;
		}
	}
	if(flag) return Flush;

	//check straight
	flag = 1;
	for(int i = 0; i < MAX_CARD-1; i++){
		if(get_value_from_index(hand[i].value) != (get_value_from_index(hand[i+1].value)) + 1){
			flag = 0;
		}
		if((get_value_from_index(hand[i].value) == 10) && (get_value_from_index(hand[i+1].value) == 1)){
			flag = 1;
		}
	}
	if(flag) return Straight;

	//check three of a kind
	if(tris) return Three_of_a_kind;

	//check two pair
	if(pair == 2)  return Two_pair;

	//check pair
	if(pair) return Pair;

	//if none return high card
	return High_card;
}

//convert enum value to string for printf use
char *score_to_string(score s){
    switch(s){
        case High_card: return "High Card";
        case Pair: return "Pair";
        case Two_pair: return "Two Pair";
        case Three_of_a_kind: return "Three of a Kind";
        case Straight: return "Straight";
        case Flush: return "Flush";
        case Full_house: return "Full House";
        case Four_of_a_kind: return "Four of a Kind";
        case Royale_flush: return "Royal Flush";
        default: return "Error";
    }
}

void print_player_cards(){
	for(int i = 0; i < 2; i++){	
		printf("%s of %s,", pg_cards[i].value, pg_cards[i].suit);
	}
}

void print_table_cards(int how_many_card){
	for(int i = 2; i <  how_many_card +2; i++){
		SLEEP(2);
		printf("%s of %s,", table_cards[i].value, table_cards[i].suit);
		fflush(stdout);
	}
}

//print the 7 picked cards and show result of the round
void print_result(score player_score, score ai_score, int plate_fiches){
	//print the ai cards
	printf("\n\nAI cards : ");
	for(int i = 0; i < 2; i++){	
		printf("%s of %s,", ai_cards[i].value, ai_cards[i].suit);
	}

    printf("\n\n");

    //convert enum score to string for better looking output
    char *pstr = score_to_string(player_score);
    char *istr = score_to_string(ai_score);

    if(player_score > ai_score){
    	printf("\nPlayer wins with a %s, and get % d fiches.\n", pstr, plate_fiches);
    	player_fiches += plate_fiches;
    }else if(player_score < ai_score){
    	printf("\nIa wins with a %s,and get %d fiches.\n", istr, plate_fiches);
    	ai_fiches += plate_fiches;
    }else{
    	switch(rand() % 3){
    		case 0: 
    			printf("\nit's a DRAW both ia and player have a %s!\nBut the house always wins *Heh-heh-heh*.\n", pstr);
    			break;
    		case 1: 
    			printf("\nit's a DRAW both ia and player have a %s!\nIn the end the house takes it all *Hmhmhmh*.\n", pstr);
    			break;
    		case 2: 
    			printf("\nit's a DRAW both ia and player have a %s!\nLuck runs out, but the house keeps running *ha-ha-ha*\n", pstr);
    			break;
    	}
    }
}
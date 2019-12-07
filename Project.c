/*
 * Project.c
 *
 *  Created on: 15 Oct 2019
 *      Author: Jayne Uni
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#define LEN 80
#define DATE 12
#define TEMP 5

typedef struct expense_tracker{
	int type;			//type income = 1, outgoing/expenditure = 0
	float amount;
	char date[DATE];
	char desc[LEN];
} TRACKER;

void print_menu(void);
void format(char *str);
void clear_all(FILE * file);
TRACKER add_income(void);
TRACKER add_expense(void);
int date_check(char *str);
int save_all(TRACKER *expense, int size, int add, FILE * file);
float print_all(TRACKER *expense, int size);
void print_income(TRACKER *expense, int size);
void print_expense(TRACKER *expense, int size);
int read_all(TRACKER * expense, int size, int save, int add, FILE * file);
int mem_check(TRACKER ** expense, int i, int n);


int main(void){
	FILE * file;
	TRACKER *expense;
	int choice, i=0, add=0, n=25, save=0;
	//i = array of structs, add = how many added before/after reading from file, save = how many elements are saved, n=memory allocated
	char temp[TEMP];
	float total = 0;

	if((expense = (TRACKER *)malloc(n*sizeof(TRACKER))) != NULL){
		if((file = fopen("ExTra.bin", "ab")) != NULL){
			do{
				print_menu();
				fgets(temp, 4, stdin);
				format(temp);
				choice = atoi(temp);
				if(choice>0 && choice<8){
					switch(choice){
					case 1 :
						clear_all(file);		//opens file in write mode
						break;
					case 2 :
						expense[i]=add_income();		//adds new struct to array
						i++;		//increment array
						add++;		//increment added elements
						n=mem_check(&expense, i, n);
						break;
					case 3 :
						expense[i] = add_expense();
						i++;
						add++;
						n=mem_check(&expense, i, n);
						break;
					case 4 :
						save=save_all(expense, i, add, file);		//how many elements were saved
						add=0;		//reset added to ensure correct elements
						break;
					case 5 :
						n=mem_check(&expense, i, n);
						if(add>save){
							printf("You have unsaved entries, loading will delete these.  Do you wish to continue? ");
							fgets(temp,TEMP,stdin);
							format(temp);
							if((strcmp(temp,"y") && (strcmp(temp,"Y")))!=0){
								break;
							}
						}
						i = read_all(expense, i, save, add, file);		//how many elements were read, array set to new value
						save=0;		//reset saved elements
						break;
					case 6 :
						total = print_all(expense, i);		//receives total of all amounts
						print_income(expense, i);
						print_expense(expense, i);
						printf("The amount of money you currently have is %.2f\n", total);
						if(total>0){
							printf("Congratulations on saving money!\n");
						} else if(total<0){
							printf("Unfortunately you are spending more money than you are earning!\n");
						}
						break;
					}
				} else {
					printf("Please enter a number between 1 and 7\n");
					choice=0;
				}
			}while(choice!=7);
		}
		if(save==0){
			printf("Would you like to save before quitting? ");
			fgets(temp,TEMP,stdin);
			format(temp);
			if((strcmp(temp,"y") && (strcmp(temp,"Y")))!=0){
				save=save_all(expense, i, add, file);
				printf("Records saved.  Goodbye.\n");
			} else {
				printf("Goodbye.\n");
			}
		}
	}
	free(expense);
	fclose(file);
}

void print_menu(void){

	printf("1. Clear all records\n");
	printf("2. Add income\n");
	printf("3. Add expenditure\n");
	printf("4. Save records to file\n");
	printf("5. Load records from file\n");
	printf("6. Print a report of all transactions\n");
	printf("7. Quit\n");
	printf("Enter a number: ");
}

void clear_all(FILE * file){
	char temp[5];

	printf("Are you sure you wish to clear all the records? (y/n) ");
	fgets(temp, 5, stdin);
	format(temp);
	if(strcmp(temp,"y") == 0){
		if((file = freopen("ExTra.bin", "wb", file)) != NULL){
			printf("Records cleared\n");
		}
	} else {
		printf("No changes made\n");
	}
	fclose(file);
}

TRACKER add_expense(){
	TRACKER expense;
	char temp[DATE];

	expense.type = 0;
	printf("Enter amount of expense: ");
	fgets(temp, DATE, stdin);
	format(temp);
	expense.amount = atof(temp);
	expense.amount *= -1;		//convert expense to minus amount
	printf("Enter description: ");
	fgets(temp, LEN, stdin);
	format(temp);
	strcpy(expense.desc, temp);
	do{
		printf("Enter date (dd/mm/yy): ");
		fgets(temp, DATE, stdin);
		format(temp);
	}while(date_check(temp) == 1);
	strcpy(expense.date, temp);

	return expense;
}


TRACKER add_income(){
	TRACKER expense;
	char temp[DATE];

	expense.type = 1;
	printf("Enter amount of income: ");
	fgets(temp, DATE, stdin);
	format(temp);
	expense.amount = atof(temp);
	printf("Enter description: ");
	fgets(temp, LEN, stdin);
	format(temp);
	strcpy(expense.desc, temp);
	do{
		printf("Enter date (dd/mm/yy): ");
		fgets(temp, DATE, stdin);
		format(temp);
	}while(date_check(temp) == 1);
	strcpy(expense.date, temp);

	return expense;
}

int date_check(char *str){
	int day, month, year;

	format(str);
	char *months[12]= {"January", "February", "March", "April", "May", "June", "July", "August",
			"September", "October", "November", "December"};
	if((sscanf(str, "%2d/%2d/%2d", &day, &month, &year) == 3) || (sscanf(str, "%2d.%2d.%2d", &day, &month, &year) == 3)){
		if(month>0 && month<=12){
			if(((month==1) || (month==3) || (month==5) || (month==7) || (month==8) || (month==10) || (month==12) == 0) && (day<1 || day>31)){
				printf("There are 31 days in %s. Please enter correct date.\n", months[month-1]);
				return 1;
			}else if((month==4 || month==6 || month==9 || month==11) &&(day<1 || day>30)){
				printf("There are 30 days in %s. Please enter correct date.\n", months[month-1]);
				return 1;
			}else if((month==2) && (day<1 || day>28)){
				printf("There are 28 days in %s. Please enter correct date.\n", months[month-1]);
				return 1;
			}else if((day==29 && month==2 && ((year+2000)%400==0 ||((year+2000)%4==0 && (year+2000)%100!=0)))){
				printf("There are only 29 days in %s during a leap year. Please enter valid date.\n", months[month-1]);
				return 1;
			}
		} else {
			printf("There are 12 months in the year. Please enter correct month.\n");
			return 1;
		}
	} else{
		printf("Error reading date.  Please re-enter.\n");
		return 1;
	}
	sprintf(str, "%02d/%02d/%02d", day, month, year);
	return 0;
}

int read_all(TRACKER * expense, int size, int save, int add, FILE * file){

	if(save>0){
		save+=add;			//if struct added after save, when loading array set back to beginning
	}
	if((file = fopen("ExTra.bin", "rb")) != NULL){
		while(!feof(file)){
			if(fread(&expense[size-save], sizeof(TRACKER), 1, file) == 1){		//reads new struct into array
				size++;
			}
		}
	} else {
		printf("Error opening file\n");
	}

	fclose(file);
	return size-save;
}

int save_all(TRACKER *expense, int size, int add, FILE * file){

	int save = 0;
	//if array has read from file, clear file and write all elements again
	if(add<size){
		fopen("ExTra.bin", "wb");
		fclose(file);
	}
	if(file != NULL){
		for(int i=0; i<size; i++){
			fopen("ExTra.bin", "ab");
			if(fwrite(&expense[i], sizeof(TRACKER), 1, file) ==1){
				save++;
			}
			fclose(file);
		}
	} else {
		printf("Error opening file\n");
	}
	return save;
}

float print_all(TRACKER *expense, int size){
	float sum=0;

	printf("%-8s | %-10s | %-80s\n", "AMOUNT", "DATE", "DESCRIPTION");
	for(int i=0; i<size; i++){
		printf("%8.2f | %10s | %-80s\n", expense[i].amount, expense[i].date, expense[i].desc);
		sum += expense[i].amount;
	}
	printf("\n");
	return sum;
}

void print_income(TRACKER *expense, int size){

	printf("%s\n", "TOTAL INCOME");
	printf("%-8s | %-10s | %-80s\n", "AMOUNT", "DATE", "DESCRIPTION");
	for(int i=0; i<size; i++){
		if(expense[i].type == 1){
			printf("%8.2f | %10s | %s\n", expense[i].amount, expense[i].date, expense[i].desc);
		}
	}
	printf("\n");
}

void print_expense(TRACKER *expense, int size){

	printf("%s\n", "TOTAL EXPENDITURE");
	printf("%-8s | %-10s | %-80s\n", "AMOUNT", "DATE", "DESCRIPTION");
	for(int i=0; i<size; i++){
		if(expense[i].type == 0){
			printf("%8.2f | %10s | %s\n", expense[i].amount, expense[i].date, expense[i].desc);
		}
	}
	printf("\n");
}

void format(char *str) {

	if (str[strlen(str) - 1] == '\n') {
		str[strlen(str) - 1] = '\0';
	}
	if (str[strlen(str) - 1] == '\r') {
		str[strlen(str) - 1] = '\0';
	}
}

int mem_check(TRACKER ** expense, int i, int n){
	TRACKER * texp;
	if(i==(n-5)){			//check if there are 5 spaces left for structs
		n += 10;			//increase memory by 10
		if((texp = realloc(expense, n *sizeof(TRACKER))) != NULL){
			expense = &texp;
		} else {
			printf("Error allocating memory.\n");
		}
	}
	return n;
}

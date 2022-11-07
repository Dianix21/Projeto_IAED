/*  
    ----------------------------------------------------------------------------
    |      Project carried out within the scope of the discipline of           |
    |           Introducao aos Algoritmos e Estrutura de Dados                 |
    |                    by: Diana Goulao IST1102531                           |
    |                                                                          |
    |             Flight Management System Between Airports                    |
    ----------------------------------------------------------------------------
*/

/* Libraries used in the project */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "structs.h"

/* Declaration of global variables */
struct flight flights[MAX_FLIGHTS];
struct flight flights_aux[MAX_FLIGHTS];
struct airport airports[MAX_AIRPORTS];
struct date sistm_data = {01, 01, 2022};
struct flight voo;

int airports_counter = 0;
int flight_counter = 0;
reservation *headReservation;
reservation *tailReservation;

int idex_f[100];

int data_cmp(struct date data_aux, struct date flight_data);


/* The update_date function takes a departure date and time and the duration of a flight and calculates the flight arrival date and time */
void update_date(int Day, int Month, int Year, int departure_hours, int departure_min, int duration_hours, int duration_min, int i){
    int arrival_day = 0, arrival_month = 0, arrival_year = 0;
    int arrival_hour = 0, arrival_minutes = 0;

    arrival_minutes = departure_min + duration_min;
    if (arrival_minutes >= 60){
        arrival_hour += 1;
        arrival_minutes -= 60;
    }

    arrival_hour = arrival_hour + departure_hours + duration_hours;
    if (arrival_hour >= 24){
        arrival_day += 1;
        arrival_hour -= 24;
    }

    arrival_day =  arrival_day  + Day;
    if ((Month == 1 || Month == 3 || Month == 5 || Month == 7 || Month == 8 
            || Month == 10 || Month == 12) && arrival_day   > 31){
        arrival_day -= 31;
        arrival_month += 1;
    }
    else if ((Month == 4 || Month == 6 || Month == 9 || Month == 11) 
        && arrival_day > 30){
        arrival_day -= 30;
        arrival_month += 1;
    }

    else if (Month == 2 && arrival_day  > 28){
        arrival_day -= 28;
        arrival_month += 1;
    }

    arrival_month = arrival_month + Month;
    if (arrival_month > 12){
        arrival_month -= 12;
        arrival_year += 1;
    }

    arrival_year = arrival_year + Year;

    flights[i].arrival_date.day = arrival_day;
    flights[i].arrival_date.month = arrival_month;
    flights[i].arrival_date.year = arrival_year;
    flights[i].arrival_time.hours = arrival_hour;
    flights[i].arrival_time.minutes = arrival_minutes;

    return;
}

/* The check_date function receives a date and returns 0 if it is invalid 
    and 1 otherwise according to certain criteria */
int check_date(int Day, int Month, int Year){
    /* criterion 1: The program raises an error if the date entered by the user is a date in the past */
    if (Year < sistm_data.year){
        return 0;
    }
    else if (Month < sistm_data.month && Year == sistm_data.year){
        return 0;
    }
    else if (Day < sistm_data.day && Month == sistm_data.month && Year == sistm_data.year){
        return 0;
    }

    /* criterion 2: The program raises an error if the date entered by the user 
        is a date more than one year in the future */
    else if (Year > sistm_data.year + 1){
        return 0;
    }
    else if (Month > sistm_data.month && Year == sistm_data.year + 1){
        return 0;
    }
    else if(Day > sistm_data.day && Month == sistm_data.month && Year == sistm_data.year + 1){
        return 0;
    }
    else{
        return 1;
    }
}


/* ---------------- Functions referring to the 'a' command -------------------*/

/* The program raises an error in case the airport identifier is not an 
        uppercase-only string */
int ID_Validation(char AirportID[]){
    int i, len_ID = strlen(AirportID);
    for (i = 0; i < len_ID; i++)
        if (!(AirportID[i] >= 'A' && AirportID[i] <= 'Z') && !(AirportID[i] >= 'a' && AirportID[i] <= 'z'))
            return 1;
    if (len_ID < 3 || len_ID > 5)
        return 1;
    return 0;
}

/* check if exist an airport created with airport ID passed by the user */
int find_Airport(char id[]){
    int i;
    for (i = 0; i < airports_counter; i++)
        if (strcmp(airports[i].IDAirport, id) == 0)
            return i;
    return -2;
}

/* Adds a new airport to the system */ 
void add_Airport (char command[]){    
    char cmd, IDAeroporto[6], Pais[MAX_COUNTRY_CHAR], Cidade[MAX_CITY_CHAR];

    sscanf(command, "%[^ ||\t] %[^ ||\t] %[^ ||\t] %[^\n]", &cmd, IDAeroporto, 
    Pais, Cidade);
    
    if (ID_Validation(IDAeroporto)){    
        printf("invalid airport ID\n");
    }
    else if (airports_counter == MAX_AIRPORTS){
        /* The program raises an error in case the airport, if created, 
            exceeds the maximum limit of airports allowed by the system */
        printf("too many airports\n");
    }
    else if (find_Airport(IDAeroporto) != -2){
        printf("duplicate airport\n");
    }
    else{
        /* With strcpy we intend to assign the information introduced about 
            the airport to be created to the elements of the structure of
            an airport that will be introduced in a vector of airports in 
            the index corresponding to airports_counter */
        strcpy(airports[airports_counter].IDAirport, IDAeroporto);
        strcpy(airports[airports_counter].country, Pais);
        strcpy(airports[airports_counter].city, Cidade);
        airports[airports_counter].flight_number = 0;
        printf("airport %s\n", IDAeroporto);
        airports_counter += 1;
    }
}

/* ---------------- Functions referring to the 'l' command -------------------*/

/* The flight_count function aims to count how many flights there are associated 
   with an airport ID */
void flights_count(int a_index){
    int j;
    for (j = 0; j < flight_counter; j++){
        if (strcmp(flights[j].departure_airport.IDAirport, 
                airports[a_index].IDAirport) == 0){
            flights[j].departure_airport.flight_number++;
        }
    }
}

void print_Airports(int a_index){
    printf("%s %s %s %d\n",airports[a_index].IDAirport, airports[a_index].city, 
        airports[a_index].country, airports[a_index].flight_number);
}

/* The sort function is an auxiliary function that aims to sort airports in alphabetical order of IDs */
void sort(){
    int i, j;
    struct airport aero_temp;
    for(i = 0; i < airports_counter - 1; i++){
        for(j = i + 1; j < airports_counter; j++){
            if(strcmp(airports[i].IDAirport,airports[j].IDAirport) > 0){
                aero_temp = airports[i];
                airports[i] = airports[j];
                airports[j] = aero_temp;
            }
        }
    }
}

/* If the command is invoked with no arguments all airports are listed 
in alphabetical order of IDs*/
void list_All_airports(){
    int j, i, flight = 0;

    sort();
    for (i = 0; i < airports_counter; i++){
        flight = 0;
        for (j = 0; j < flight_counter; j++){
            if (strcmp(flights[j].departure_airport.IDAirport,
                airports[i].IDAirport) == 0){
                    flight += 1;
            }
        }
        printf("%s %s %s %d\n",airports[i].IDAirport, airports[i].city, 
            airports[i].country, flight);
    }
}

/* If the command is invoked with a list of airport identifiers, airports 
are listed in the order of their identifiers in the command */
void list_specific_airports(int cmd_Len, char command[]){
    int i, a_index;
    int tmpCommandIndex = 0;
    char airport_ID[4];

    for (i = 2; i<= cmd_Len; i++){
        if(command[i] == ' ' || i == cmd_Len){
            tmpCommandIndex = 0;
            a_index = find_Airport(airport_ID);
            if(a_index == -2){
                printf("%s: no such airport ID\n", airport_ID);
                continue;
            }
            else{
                flights_count(a_index);
                print_Airports(a_index);
                continue;
            }         
        }
        airport_ID[tmpCommandIndex] = command[i];
        tmpCommandIndex += 1;
    }
}

/* list the airports */
void list_Airports(char command[]){ 
    int command_len = strlen(command);

    if (command_len == 1)
        list_All_airports();
    else
        list_specific_airports(command_len, command);
}

/* ---------------- Functions referring to the 'v' command -------------------*/ 

void print_Flights(){
    int i;
    for (i = 0; i < flight_counter; i++){
        printf("%s %s %s %02d-%02d-%04d %02d:%02d\n", 
            flights[i].flight_code.string_code, 
            flights[i].departure_airport.IDAirport, 
            flights[i].arrival_airport.IDAirport, flights[i].date.day, 
            flights[i].date.month, flights[i].date.year, 
            flights[i].departure_time.hours, flights[i].departure_time.minutes);
    }
}

/* The program raises an error in case the flight code is not a string 
with two capital letters followed by digits (between 1 and 4 digits) */
int Flight_Code_Validation(char codigo_voo[]){
    int i = 0, len_code = strlen(codigo_voo);
    if (len_code < 3 || len_code > 6)
        return 0;
    for(i = 0; i < 2; i++)
        if (isdigit(codigo_voo[i]) || islower(codigo_voo[i]))
            return 0;
    for(i = 2; i < len_code; i++)
        if (!(codigo_voo[i] >= '0' && codigo_voo[i] <= '9'))
            return 0;
    if (codigo_voo[2] == '0')
        return 0;
    return 1; 
}

/* The program raises an error in case there is already a flight with 
the same code for the same day */
int find_flight(char codigo_voo[], struct date data_aux){
    int i;
    for (i = 0; i < flight_counter; i++){
        if (strcmp(flights[i].flight_code.string_code, codigo_voo) == 0){
            if(flights[i].date.day == data_aux.day && 
                flights[i].date.month == data_aux.month && 
                flights[i].date.year == data_aux.year){
                return i;
            }
        }
    }
    return -2;
}

/* Auxiliary function that allows checking the validity of a flight to be created */
int flight_Validation(struct flight voo){
    if (Flight_Code_Validation(voo.flight_code.string_code) == 0)
        printf("invalid flight code\n");
    else if (find_flight(voo.flight_code.string_code, voo.date)!= -2)
        printf("flight already exists\n");
    else if (find_Airport(voo.arrival_airport.IDAirport) == -2)
        printf("%s: no such airport ID\n", voo.arrival_airport.IDAirport);
    else if (find_Airport(voo.departure_airport.IDAirport) == -2)
        printf("%s: no such airport ID\n", voo.departure_airport.IDAirport);
    else if (flight_counter == MAX_FLIGHTS)
        /* The program raises an error in case the new flight to be 
            created exceeds the flight limit. */
        printf("too many flights\n");
    else if (check_date(voo.date.day, voo.date.month, voo.date.year) == 0)
        printf("invalid date\n");
    else if(voo.flight_duration.hours > 12 || 
        (voo.flight_duration.hours == 12 && voo.flight_duration.minutes > 0))
        /* The program raises an error if the user is trying to create a flight 
            lasting more than 12 hours. */
        printf("invalid duration\n");
    else if (voo.number_of_passengers < 10)
        /* The program raises an error in case the capacity entered is not an 
        integer greater than 10 (inclusive). */
        printf("invalid capacity\n");
    else
        return 1;
    return -2;
}

void create_flight(struct flight voo){
    strcpy(flights[flight_counter].flight_code.string_code, voo.flight_code.string_code);
    strcpy(flights[flight_counter].departure_airport.IDAirport, voo.departure_airport.IDAirport);
    strcpy(flights[flight_counter].arrival_airport.IDAirport, voo.arrival_airport.IDAirport);
    flights[flight_counter].date.day = voo.date.day;
    flights[flight_counter].date.month = voo.date.month;
    flights[flight_counter].date.year = voo.date.year;
    flights[flight_counter].departure_time.hours = voo.departure_time.hours;
    flights[flight_counter].departure_time.minutes = voo.departure_time.minutes;
    flights[flight_counter].flight_duration.hours = voo.flight_duration.hours;
    flights[flight_counter].flight_duration.minutes = voo.flight_duration.minutes;
    flights[flight_counter].number_of_passengers = voo.number_of_passengers;

    update_date(flights[flight_counter].date.day, 
        flights[flight_counter].date.month, flights[flight_counter].date.year, 
        flights[flight_counter].departure_time.hours, 
        flights[flight_counter].departure_time.minutes,
        flights[flight_counter].flight_duration.hours, 
        flights[flight_counter].flight_duration.minutes, flight_counter);
    
    flight_counter += 1;
}

/*add a flight or list all flights*/
void add_List_Flights(char command[]){ 
    int cmd_Len = strlen(command);    
    char cmd;
    
    if (cmd_Len == 1){
        print_Flights();
        return;
    }

    else{
        sscanf(command, "%[^ ] %[^ ] %[^ ] %[^ ] %02d-%02d-%04d %02d:%02d %02d:%02d %d[^\n]", 
            &cmd, voo.flight_code.string_code, voo.departure_airport.IDAirport,
            voo.arrival_airport.IDAirport, &voo.date.day, 
            &voo.date.month, &voo.date.year, &voo.departure_time.hours, 
            &voo.departure_time.minutes, &voo.flight_duration.hours,
            &voo.flight_duration.minutes, &voo.number_of_passengers);

        if (flight_Validation(voo) != -2){
            create_flight(voo);
        }
    }
}

/* ---------------- Functions referring to the 'p' command -------------------*/         

/* The flight_sort helper function sorts by departure date and time, that is, 
flights are sorted from oldest to newest date and time. */
void departure_flight_sort(int counter_aux){
    int i, j;
    struct flight flight_temp;
    for(i = 0; i < counter_aux - 1; i++){
        for(j = i + 1; j < counter_aux; j++){
            if(flights_aux[i].date.year * 100000000 +  
               flights_aux[i].date.month * 1000000 + 
               flights_aux[i].date.day * 10000 + 
               flights_aux[i].departure_time.hours * 100 + 
               flights_aux[i].departure_time.minutes
            > flights_aux[j].date.year * 100000000 +  
               flights_aux[j].date.month * 1000000 + 
               flights_aux[j].date.day * 10000 + 
               flights_aux[j].departure_time.hours * 100 + 
               flights_aux[j].departure_time.minutes){

                flight_temp = flights_aux[i];
                flights_aux[i] = flights_aux[j];
                flights_aux[j] = flight_temp;
            }
        }
    }
}

/* Lists flights departing from an airport*/
void departure_Flight_list(char command[]){ 
    int i, j = 0;
    char cmd, ID[4];

    sscanf(command, "%[^ ||\t] %[^'\n']", &cmd, ID);

    if(find_Airport(ID) == -2)
        printf("%s: no such airport ID\n",ID);

    for (i = 0; i < flight_counter; i++){
        if (strcmp(flights[i].departure_airport.IDAirport, ID) == 0 && 
        data_cmp(flights[i].date, sistm_data) == 1)
            flights_aux[j++] = flights[i];
    }

    departure_flight_sort(j);

    for (i = 0; i < j; i++){
        printf("%s %s %02d-%02d-%04d %02d:%02d\n", 
            flights_aux[i].flight_code.string_code, 
            flights_aux[i].arrival_airport.IDAirport, 
            flights_aux[i].date.day, flights_aux[i].date.month, 
            flights_aux[i].date.year, flights_aux[i].departure_time.hours, 
            flights_aux[i].departure_time.minutes);
    }
}

/* ---------------- Functions referring to the 'c' command -------------------*/

/* The arrival_flight_sort helper function sorts by arrival date and time, 
that is, flights are sorted from oldest to newest date and time. */
void arrival_flight_sort(int flight_counter){
    int i, j;
    struct flight flight_temp;
    for(i = 0; i < flight_counter - 1; i++){
        for(j = i + 1; j < flight_counter; j++){
            if(flights_aux[i].arrival_date.year * 100000000 +  
               flights_aux[i].arrival_date.month * 1000000 + 
               flights_aux[i].arrival_date.day * 10000 + 
               flights_aux[i].arrival_time.hours * 100 + 
               flights_aux[i].arrival_time.minutes
            > flights_aux[j].arrival_date.year * 100000000 +  
               flights_aux[j].arrival_date.month * 1000000 + 
               flights_aux[j].arrival_date.day * 10000 + 
               flights_aux[j].arrival_time.hours * 100 + 
               flights_aux[j].arrival_time.minutes){

                flight_temp = flights_aux[i];
                flights_aux[i] = flights_aux[j];
                flights_aux[j] = flight_temp;
            }
        }
    }
}

/* lists flights arriving at an airport */
void list_Flights_Arrival(char command []){ 
    int i, j=0;
    char cmd, ID[4];

    sscanf(command, "%[^ ||\t] %[^\n]", &cmd, ID);

    if (find_Airport(ID) == -2)
        printf("%s: no such airport ID\n",ID);    

    for (i = 0; i < flight_counter; i++){
        if (strcmp(flights[i].arrival_airport.IDAirport,ID) == 0)
            flights_aux[j++] = flights[i];
    }

    arrival_flight_sort(j);

    for (i = 0; i < j; i++){
        printf("%s %s %02d-%02d-%04d %02d:%02d\n", 
        flights_aux[i].flight_code.string_code, 
        flights_aux[i].departure_airport.IDAirport, 
        flights_aux[i].arrival_date.day, flights_aux[i].arrival_date.month, 
        flights_aux[i].arrival_date.year, flights_aux[i].arrival_time.hours, 
        flights_aux[i].arrival_time.minutes);
    }
}

/* ---------------- Functions referring to the 't' command -------------------*/

/* advance the system date */
void change_Date(char command[]){ 
    char cmd;
    int Day, Month, Year;

    sscanf(command,"%01c %02d-%02d-%04d", &cmd, &Day, &Month, &Year);

    if(check_date(Day, Month, Year) == 0) 
        /* The program raises an error if the entered date is invalid */
        printf("invalid date\n");
    else{ 
        /*If the date entered is valid, the program prints the date in DD-MM-YYYY format*/
        printf("%02d-%02d-%04d\n", Day, Month, Year);
    
        sistm_data.day = Day;
        sistm_data.month = Month;
        sistm_data.year = Year;
    }
}

/* ---------------- Functions referring to the 'r' command -------------------*/


/* Auxiliar function that takes two dates and returns 1 if they are equal and 0 otherwise */
int data_cmp(struct date data_aux, struct date flight_data){
    if (data_aux.day == flight_data.day && data_aux.month == flight_data.month &&
        data_aux.year == flight_data.year)
        return 1;    
    return 0;
}

/* Prints each reservation on the flight with the code <flight_code> on the date <data_aux>. 
 One reservation per line in the lexicographical order of the reservation code.*/
void print_Reservation(char flight_code[], struct date data_aux){
    reservation *cur_reservation = headReservation;

    if(cur_reservation == NULL){ /* in which case there is no reservation created */
        return;
    }
    
    do {
      if(strcmp(cur_reservation -> flight_code.string_code, flight_code) == 0
        && data_cmp(data_aux, cur_reservation -> data)){
        printf("%s %d\n", cur_reservation -> reservation_code, 
            cur_reservation -> number_of_passengers);
      }
      cur_reservation = cur_reservation -> next_reserv;
    }while(cur_reservation != NULL);
}

/* If the 'r' command is invoked only with the <flight_code> and <data_aux>, all 
reservations referring to the flight are listed with the code <flight_code> 
on the date <data_aux> */
void list_All_reservation(char command[]){
    char cmd, flight_code[7];
    struct date data_aux;

    sscanf(command,"%01c %[^ ||\t] %02d-%02d-%04d", &cmd, flight_code, &data_aux.day,
            &data_aux.month, &data_aux.year);

    if (find_flight(flight_code, data_aux) == -2)
        printf("%s: flight does not exist\n", flight_code);
    else if (check_date(data_aux.day, data_aux.month, data_aux.year) == 0)
        printf("invalid date\n");
    else
        print_Reservation(flight_code, data_aux);
}

/*The program raises an error in case the reservation code is not a string 
composed only of uppercase and digits or if it is composed of less than 10 characters.*/
int r_code_Validation(char r_code[]){
    int i, code_len = strlen(r_code);
    if(code_len < 10)
        return -2;
    for (i = 0; i < code_len; i++)
        if (!(r_code[i] >= '0' && r_code[i] <= '9') && 
                !(r_code[i] >= 'A' && r_code[i] <= 'Z'))
                return -2;
    return 1;
}

/* The program raises an error in case there is already a reservation 
with the indicated <r_code> */
int find_r_code(char r_code[]){
    reservation *cur_reservation = headReservation;

    if(headReservation == NULL)
        return -2;

    else{
        do {
            if(strcmp(cur_reservation -> reservation_code, r_code) == 0){
                return 1;
            }
            cur_reservation = cur_reservation -> next_reserv;
        }while(cur_reservation != NULL);
        
        return -2;
    }
}

/* The program raises an error in case the reservation to be created exceeds the flight capacity */
int check_capacity(char f_code[], struct date data_aux, int n){
    int flight_index = find_flight(f_code, data_aux);
      
    if((flights[flight_index].reserv_number + n) <= 
        flights[flight_index].number_of_passengers){
        flights[flight_index].reserv_number += n;
        return 1;
    }
    return -2;
}

/* Auxiliary function that allows checking the validity of a reservation to be created */
int reserv_Validation(char f_code[], struct date data_aux, char r_code[], int n){
    if (r_code_Validation(r_code) == -2)
        printf("invalid reservation code\n");
    else if (find_flight(f_code, data_aux) == -2)
        printf("%s: flight does not exist\n", f_code);
    else if (find_r_code(r_code) != -2)
        printf("%s: flight reservation already used\n", r_code);
    else if (check_capacity(f_code, data_aux, n) == -2)
        printf("too many reservations\n");
    else if (check_date(data_aux.day, data_aux.month, data_aux.year) == 0)
        printf("invalid date\n");
    else if (n <= 0)
        /* The program raises an error in case the number of passengers is not 
        an integer greater than 0*/
        printf("invalid passenger number\n");
    else
        return 1;
    return -2;
}

/* Auxiliary function that allows adding a new reservation to the linked list */
void add_new_node(reservation *new_reservation){
    if(headReservation == NULL){ /* List is empty, add as head */
        headReservation = new_reservation;
        headReservation ->next_reserv = NULL;
        headReservation -> prev_reserv = NULL;
        tailReservation = new_reservation;
        tailReservation ->next_reserv = NULL;
        tailReservation -> prev_reserv = NULL;
    } 
    else{    
        reservation *aux, *curReservation = headReservation;
        int inserted = 0;
        do{
            if(strcmp(curReservation-> reservation_code, new_reservation -> reservation_code) > 0){
                new_reservation-> next_reserv = curReservation;
                new_reservation-> prev_reserv = curReservation -> prev_reserv;
                curReservation -> prev_reserv = new_reservation;
                
                if(strcmp(curReservation->reservation_code,headReservation->reservation_code) == 0)
                    headReservation = new_reservation;
                else{
                    aux = new_reservation -> prev_reserv;
                    aux -> next_reserv = new_reservation;
                }

                inserted = 1;
                if (inserted)
                    break;
            }
            curReservation = curReservation -> next_reserv;
        }while( curReservation != NULL);

        if(!inserted){ /* insert at the end of the list */
            tailReservation -> next_reserv = new_reservation;
            new_reservation -> prev_reserv = tailReservation;
            new_reservation -> next_reserv = NULL;
            tailReservation = new_reservation;
        }
    }
}

/* If the 'r' command is invoked with all arguments, a new reservation is created if it is valid */
void create_Reservation(char command[]){
    char cmd, f_code[7], r_code_b[65535], *r_code;
    int n_passangers, len;
    struct date data_aux;
    reservation *new_reservation;

    sscanf(command,"%01c %[^ ||\t] %02d-%02d-%04d %[^ ||\t] %d", &cmd, 
        f_code, &data_aux.day, &data_aux.month, &data_aux.year, r_code_b,
        &n_passangers);
    len = strlen(r_code_b);
    r_code = malloc(sizeof(char)*(len+1));
    strcpy(r_code,r_code_b);
    if (reserv_Validation(f_code, data_aux, r_code, n_passangers)!= -2){
        new_reservation = malloc(sizeof(reservation));
        strcpy(new_reservation -> flight_code.string_code, f_code);
        new_reservation -> data = data_aux;
        new_reservation -> reservation_code = malloc(sizeof(char)*(len+1));
        strcpy(new_reservation -> reservation_code, r_code); 
        new_reservation -> number_of_passengers = n_passangers;
        add_new_node(new_reservation);
        free(r_code);
    }
}

/* Add a reservation or list the reservations for a flight */
void add_List_Reservation(char command[]){ 
    int cmd_len = strlen(command);
    
    if (cmd_len >= 16 && cmd_len <= 19)
        list_All_reservation(command);
    else
        create_Reservation(command);
}

/* ---------------- Functions referring to the 'e' command -------------------*/
int* index_f_code(char f_code[]){
    int i, j = 0;
    for (i = 0; i < flight_counter; i++)
        if (strcmp(flights[i].flight_code.string_code, f_code) == 0)
            idex_f[j++] = i;
    return idex_f;
}


/* auxiliary function that returns the index of a flight with <f_code> 
if not found, returns -2 */
int find_f_code(char f_code[]){
    int i;
    for (i = 0; i < flight_counter; i++)
        if (strcmp(flights[i].flight_code.string_code, f_code) == 0)
            return i;
    return -2;
}

/* auxiliary function updates the number of reservations for a flight when they are deleted */
void update_n_reserv(int n_passengers, char f_code[], struct date data_aux){
    int flight_index = find_flight(f_code, data_aux);

    flights[flight_index].reserv_number -= n_passengers;
    return;
}

/* auxiliary function that deletes a reservation from the linked list */
void delete_reservation(char code[]){
    reservation *aux,*aux2, *curReservation = headReservation;
    
    if(curReservation == NULL){ /* in which case there is no reservation created */
        return;
    }
    else if (strcmp(headReservation-> reservation_code, tailReservation -> reservation_code) == 0){ /*check whether the list has only one node*/
        update_n_reserv(curReservation -> number_of_passengers, 
            curReservation->flight_code.string_code, curReservation->data);
        headReservation = NULL;
        tailReservation = NULL;
    }
    else{ /* general case */
        do {
            if(strcmp(curReservation->reservation_code, code) == 0){
                update_n_reserv(curReservation -> number_of_passengers, 
                    curReservation->flight_code.string_code, curReservation->data);

                if(strcmp(curReservation-> reservation_code, headReservation -> reservation_code) == 0){ /* delete the first element from the list */
                    headReservation = curReservation -> next_reserv;
                    headReservation -> prev_reserv = NULL;
                    if (headReservation -> next_reserv == NULL){
                        headReservation = tailReservation;
                        free(curReservation);
                        return;
                    }
                 }
                else {
                    aux = curReservation ->prev_reserv;
                    aux->next_reserv=curReservation->next_reserv;
                }
                
                if (curReservation -> next_reserv == NULL){ /* delete the last element in the list */
                    tailReservation = curReservation -> prev_reserv;
                    aux = curReservation -> prev_reserv;
                    aux->next_reserv = curReservation -> next_reserv;
                    free(curReservation);
                    return;
                }

                aux2 = curReservation-> next_reserv;
                aux2 -> prev_reserv = curReservation ->prev_reserv;
                free(curReservation);
                curReservation = NULL;
                return;
            }
            curReservation = curReservation -> next_reserv;

        }while(curReservation != NULL);
    }
}

/* auxiliary function that erases a flight */
void delete_flight(int f_index){
    int i;
    for (i = f_index; i < flight_counter - 1; i++)
        flights[i] = flights[i + 1];
    memset(&flights[flight_counter - 1], 0, sizeof *flights);
    flight_counter -= 1 ;
}

/* auxiliary function that allows to delete flight reservations with <f_code> in date <data_aux> */
void delete_specific_reserv(char f_code[], struct date data_aux){ 
    reservation *cur_reservation = headReservation;

    if(headReservation == NULL)
        return;

    else{
        do {
            if(strcmp(cur_reservation -> flight_code.string_code, f_code) == 0 && 
            data_cmp(cur_reservation->data, data_aux)){
                delete_reservation(cur_reservation->reservation_code);
            }
            cur_reservation = cur_reservation -> next_reserv;
        }while(cur_reservation != NULL);
        
        return;
    }
}

/* delete flights or reservation */
void delete_flight_reservation(char command[]){ 
    char cmd, code[500];
    int f_index, j;
    struct date data_aux;
    sscanf(command, "%c %s", &cmd, code);

    f_index = find_f_code(code);
    data_aux = flights[f_index].date;

    if (find_r_code(code) == -2 && f_index == -2)
        printf("not found\n");
    else if(find_r_code(code) != -2)
        delete_reservation(code);
    else{
        while (idex_f[j++] != '\0'){
            if (check_date(data_aux.day, data_aux.month, data_aux.year) == 0){
                delete_specific_reserv(code, data_aux);
                delete_flight(idex_f[j]);
            }
            printf("not found");
        }
    }
}

/* Function void leLinha(char s[]): receive a string and as the function itself 
    indicates, this one serves to read a complete line of input */
void le_Linha(char s[]){
    int c, i = 0;
    while ((c = getchar()) != '\n' && c != EOF){
        s[i++] = c;
    }
    s[i] = '\0';
}

/*Function int main(): reads from the terminal (standard input) the instructions
     in the form of a set of lines starting with a character, 
     which is now called command*/
int main(){
    char command[65535];
    while(1){
        le_Linha(command);
        switch (command[0]){
        case 'q': return 0;
        case 'a': add_Airport(command);
            break;
        case 'l': list_Airports(command);
            break;
        case 'v': add_List_Flights(command);
            break;
        case 'p': departure_Flight_list(command);
            break;
        case 'c': list_Flights_Arrival(command);
            break;
        case 't': change_Date(command);
            break;
        case 'r': add_List_Reservation(command);
            break;
        case 'e': delete_flight_reservation(command);
            break;
        default: printf("Invalid comand: %c\n", command[0]);
        }
    }
    return 0;
}
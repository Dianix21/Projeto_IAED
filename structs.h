/*  
    ----------------------------------------------------------------------------
    |      Project carried out within the scope of the discipline of           |
    |           Introducao aos Algoritmos e Estrutura de Dados                 |
    |                    by: Diana Goulao IST1102531                           |
    |                                                                          |
    |             Flight Management System Between Airports                    |
    ----------------------------------------------------------------------------
*/

/* Declaration of the constants used throughout the project, attention that as these are string vector
    sizes, the value of the constant corresponds to the original value plus one */
    
#define MAX_AIRPORTS 40    /* maximum number of airports */
#define MAX_FLIGHTS 30000   /* maximum number of flights */
#define MAX_COUNTRY_CHAR 31    /* country name dimension */
#define MAX_CITY_CHAR 51    /* city name dimension */
#define MAX_FLIGHT_HOURS 12    /* maximum number of flight hours */

/* Declaration of the structures used in the project */
struct airport{
    char IDAirport[7];
    char country[MAX_COUNTRY_CHAR];
    char city[MAX_CITY_CHAR];
    int flight_number;
};

struct date{
    int day;
    int month;
    int year;
};

struct time{
    int hours;
    int minutes;
};

struct flight_code{
    char string_code[7];
};

struct flight{
    struct flight_code flight_code;
    struct airport departure_airport;
    struct airport arrival_airport;
    struct date date;
    struct time departure_time;
    struct time flight_duration;
    int number_of_passengers;
    struct date arrival_date;
    struct time arrival_time;
    int reserv_number;
};

typedef struct _reservation{
    struct flight_code flight_code;
    struct date data;
    char *reservation_code;
    int number_of_passengers;
    struct _reservation *prev_reserv;
    struct _reservation *next_reserv;
} reservation;
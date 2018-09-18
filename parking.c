#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>  // Libreria strtol
#include <unistd.h>

void estadoParking (int plazas, int * parking){
    printf("Estado del parking:\n");
    for(int i = 0; i<plazas;i++){
        printf("[%d]",parking[i]);  
    }
    printf("\n");   
}

int main(int argc, char** argv) {
    //Datos que necesitaremos para la ejecución
    int plazas = strtol(argv[1], NULL, 10);
   
    printf("El numero de plazas es %d\n", plazas);
    
    int parking[plazas];
    int plaza = -1; // Valor a devolver en caso de que no haya sitio en el parking
    int lleno=0; // Valor que nos ayudará en la lógica de asignación de plaza
    int ocupado = 0; // Valor que nos ayudará en la lógica de asignación de plaza
    MPI_Status Stat;
	
    //Inicializar valores parking
    for(int i = 0; i<plazas;i++){
        parking[i]= 0;
    }
	
    // Inicializar MPI
    MPI_Init(NULL, NULL);
	
	
    int inmsg[3]; //Array donde guardaremos lo que recibimos
    
    MPI_Barrier(MPI_COMM_WORLD);
    while(1){
        MPI_Recv(inmsg, 3, MPI_INT, MPI_ANY_SOURCE,0, MPI_COMM_WORLD, &Stat);
        lleno = 1;

       	char* in_out = "Entrar";
	char* car_truck = "Coche";
	
	if(inmsg[1] == 1)//Quiere salir
		in_out = "Salir";
	if(inmsg[2] == 1)//Es un camion
		car_truck = "Camión";



        printf(" Numero de proceso: %d, Entrar/Salir: %s, Coche/Camión: %s\n", inmsg[0], in_out, car_truck);
        
        if(inmsg[1] == 0){//Quiere entrar
            if(inmsg[2] == 0){//Es un coche
                for(int i = 0; i<plazas;i++){
                    if(parking[i]==0 && ocupado == 0){ //Hay plaza
                        parking[i] = inmsg[0];
                        printf("El coche %d ha aparcado en la posición %d\n", inmsg[0], i);
                        ocupado = 1;
                        lleno = 0;
                        MPI_Send(&i, 1, MPI_INT, inmsg[0], 0, MPI_COMM_WORLD);
                    } 
                }
                if(lleno == 1){ //No hay plazas
                    printf("No hay plazas, esperando en cola\n");
                    MPI_Send(&plaza, 1, MPI_INT, inmsg[0], 0, MPI_COMM_WORLD);
                }
                ocupado = 0;
            }
            else if(inmsg[2]==1){//Es un camión
                for (int i = 0;i<(plazas-1);i++){
                    if(parking[i] == 0 && parking[i+1]==0 && ocupado == 0){//Hay dos plazas para nuetro señor camion
                        parking[i] = inmsg[0]; 
                        parking[i+1] = inmsg[0];
                         printf("El camion %d ha aparcado en las posiciónes %d y %d\n", inmsg[0], i, i+1);
                        ocupado = 1;
                        lleno = 0;
                        MPI_Send(&i, 1, MPI_INT, inmsg[0], 0, MPI_COMM_WORLD);
                    }
                }
                if(lleno == 1){//No hay plazas para el camión
                    printf("No hay plazas, esperando en cola\n");
                    MPI_Send(&plaza, 1, MPI_INT, inmsg[0], 0, MPI_COMM_WORLD);
                }
                ocupado = 0;
            }
        }
        else if(inmsg[1] == 1){//Quiere salir
            if(inmsg[2] == 0){//es un coche
                for(int i = 0; i<plazas;i++){
                    if(parking[i]==inmsg[0]){
                        parking[i] = 0;
                        printf("El coche %d ha dejado la plaza %d libre\n", inmsg[0], i);
                    }
                }
            }
            else if(inmsg[2] = 1){//Es un camion
                for (int i = 0; i<plazas;i++){
                    if (parking[i]==inmsg[0]){
                        parking[i] = 0;
                        parking[i+1] = 0;
                        printf("El camion %d ha dejado las plazas %d y %d libres\n", inmsg[0],i,i+1);
                    }
                }
                
            }
        }
        ocupado = 0;
        estadoParking(plazas, parking);
        sleep(1);
    }

   //Fizalizar MPI
    MPI_Finalize();
}


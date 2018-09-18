#include <mpi.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv) {
 
    //Datos que necesitaremos durante la ejecución del programa
    int dest = 0, inmsg;
    MPI_Status Stat;
    /*
     *Usaremos una array de tres posiciones que encviaremos al parking
     * [0] es el número de proceso
     * [1] sera 0 si queremos entrar y 1 si queremos salir
     * [2] sera 0 si somos un coche y 1 si somos un camión
     */
    int outmsg[3];
    
    //Inicializar
    MPI_Init(NULL, NULL);

    // Obtener numero de proceso
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    //Introducimos valores correspondientes en nuestro array a enviar
    outmsg[0] = world_rank; //Numero de proceso
    outmsg[1] = 0 ; // nuestra primera opción sera entrar
    outmsg[2] = 1 ; // 1 indica que es un camion
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    while(1){
        
    sleep(rand() %15);//Tiempo aleatorio antes de entrar  
        
    outmsg[1] = 0; //Queremos entrar
    
    //Enviamos nuestro PID pidiendo plaza en el parking
    do   {  
        MPI_Send(outmsg, 3, MPI_INT, dest, 0, MPI_COMM_WORLD);
        MPI_Recv(&inmsg, 1, MPI_INT, MPI_ANY_SOURCE,0, MPI_COMM_WORLD, &Stat); //Recibe la plaza
    } while ( inmsg < 0 );//Si la plaza es -1 no hay hueco y volvemos a pedir
    
    
    //Dormir entre 2 y 8 segundos
    sleep((rand() % 6)+2);
    
    outmsg[1] = 1; //Despues del random sleep queremos salir
            
    MPI_Send(outmsg, 3, MPI_INT, dest, 0, MPI_COMM_WORLD);
            
    }
	
    MPI_Barrier(MPI_COMM_WORLD);
    
    // Finalizar MPI
    MPI_Finalize();
}

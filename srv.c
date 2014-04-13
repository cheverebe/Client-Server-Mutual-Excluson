#include "srv.h"

#define TAG_PEDIDO_S          21    /* srv -> srv */
#define TAG_OTORGADO_S        31    /* srv -> srv */
#define TAG_LIBERO_S          41    /* srv -> srv */
#define TAG_TERMINE_S         51    /* srv -> srv */


void servidor(int mi_cliente)
{
    int mi_secuencia=0;
	int secuencia_ajena=0;
	int secuencia_enviada=0;
    int servers[cant_ranks/2];
    int rta_pendiente[cant_ranks/2];
    int otorgados = 0;
    int necesarios = cant_ranks/2 -1;
    int i;
    int clientes_vivos = cant_ranks/2;

    MPI_Status status;
    int origen, tag;
    int tengo_el_mutex = FALSE;
    int hay_pedido_local = FALSE;
    int listo_para_salir = FALSE;

    for(i = 0; i < cant_ranks/2; i++) {
        servers[i]=2*i;
        rta_pendiente[i]=FALSE;
    }
    
    while( ! listo_para_salir ) {
        
        MPI_Recv(&secuencia_ajena, 1, MPI_INT, ANY_SOURCE, ANY_TAG, COMM_WORLD, &status);
        origen = status.MPI_SOURCE;
        tag = status.MPI_TAG;
        //me dice q le consiga el mutex (mi cliente)
        if (tag == TAG_PEDIDO) {
            debug("Mi cliente solicita acceso exclusivo");
            assert(origen == mi_cliente);
            assert(hay_pedido_local == FALSE);

			//seteo el flag de que tengo un pedido
			hay_pedido_local = TRUE;
			//checkeo que no este solo
            if (cant_ranks > 2) {

				mi_secuencia++;

				//le mando a los demas el pedido de mutex
                for (i=0; i < cant_ranks/2; i++) {
                    if (servers[i]!=mi_rank) {
                        secuencia_enviada = mi_secuencia;
						MPI_Send(&mi_secuencia, 1, MPI_INT, servers[i], TAG_PEDIDO_S, COMM_WORLD);
                    }
                }
            } else {
				//le doy permiso porque hay solo un servidor
				MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
			}
		}
        
        //me piden el mutex (OTRO SERVER)
        else if (tag == TAG_PEDIDO_S) {			
			/*
			Solo le doy permiso solo si se cumplen estas cosas
				+ No tengo el mutex
				+ Si no tengo pedido de mi cliente, o si lo tengo pero el otro servidor me gana en prioridad 	
			*/
            if((!hay_pedido_local && !tengo_el_mutex) ||
                    ( hay_pedido_local && (!tengo_el_mutex) &&
                      (secuencia_enviada > secuencia_ajena ||
                       ( secuencia_enviada == secuencia_ajena && origen < mi_rank ) ) ) ) {
    			debug("les doy permiso a los SERVERS de menor ranking");
    			MPI_Send(NULL, 0, MPI_INT, origen, TAG_OTORGADO_S, COMM_WORLD);
            } else {
                //si no le di permiso marco como pendiente
                rta_pendiente[origen/2] = TRUE;
            }
			//actualizo el numero de secuencia
            if (mi_secuencia <= secuencia_ajena) {
                mi_secuencia=secuencia_ajena +1;
            }
        }
        //me avisan q me otorgan el mutex (OTRO SERVER)
        else if (tag == TAG_OTORGADO_S) {
            otorgados++;
			//cuando todos me dieron permiso
            if(otorgados == necesarios) {
                otorgados = 0;
                debug("Dándole permiso (A MI CLIENTE)");

                //marco que ya tengo el mutex para no darle permiso a nadie mas hasta que lo libere
                tengo_el_mutex = TRUE;
                MPI_Send(NULL, 0, MPI_INT, mi_cliente, TAG_OTORGADO, COMM_WORLD);
            }
        }
        // mi cliente me dice que le avise a los demas que el muex queda libre
        else if (tag == TAG_LIBERO) {
            debug("Mi cliente libera su acceso exclusivo");
            assert(origen == mi_cliente);
            assert(hay_pedido_local == TRUE);

			//libero los flags de pedido y mutex
            hay_pedido_local = FALSE;
            tengo_el_mutex = FALSE;
            //le doy permiso a los que quedaron pendientes
            for (i = 0;i < cant_ranks/2; i++) {
                if (rta_pendiente[i] == TRUE) {
                    rta_pendiente[i] =FALSE;
                    MPI_Send(NULL, 0, MPI_INT, servers[i], TAG_OTORGADO_S, COMM_WORLD);
                }
            }
        }
        // mi cliente me avisa q no me necesita mas
        else if (tag == TAG_TERMINE) {
            assert(origen == mi_cliente);
            assert(hay_pedido_local == FALSE);
            assert(tengo_el_mutex == FALSE);

            clientes_vivos--;

            debug("Mi cliente avisa que terminó");
			//le aviso a los demas que mi cliente quiere terminar para que actualicen su contador
            for (i = 0; i < cant_ranks/2; i++) {
                if (servers[i] != mi_rank) {
                    secuencia_enviada = mi_secuencia;
                    MPI_Send(NULL, 0, MPI_INT, servers[i], TAG_TERMINE_S, COMM_WORLD);
                }
            }
            //cierro el server cuando todos los clientes terminaron para evitar que queden mensajes colgados
            if (clientes_vivos == 0) {
                assert(tengo_el_mutex == FALSE);
                assert(hay_pedido_local == FALSE);
                listo_para_salir = TRUE;
            }
        }
        // me avisan q se murio un cliente
        else if (tag == TAG_TERMINE_S) {
            clientes_vivos--;
            debug("otro server me avisa que su cliente terminó");
            //cierro el server cuando todos los clientes terminaron para evitar que queden mensajes colgados
            if (clientes_vivos == 0) {
                listo_para_salir = TRUE;
            }
        }
    }
}


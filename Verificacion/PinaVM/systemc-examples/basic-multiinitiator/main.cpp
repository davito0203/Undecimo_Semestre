/********************************************************************
 * Copyright (C) 2009 by Ensimag and Verimag                        *
 * Initial author: Matthieu Moy                                     *
 ********************************************************************/

/* Plateforme SystemC utilisant le protocole TLM BASIC.

   Cette version est en un seul fichier pour faciliter la
   compréhension. Une version multi-fichiers (donc plus propre) est
   également disponible.
*/

#include "basic.h"
#include "bus.h"


using namespace std;
using namespace sc_core;

struct initiator : sc_module {
        basic::initiator_socket<initiator> socket;
        void thread(void) {
		basic::addr_t addr = 4;
        cout << name() << " write to Bob at address " << addr << endl;
		for (basic::data_t val = 1; val <= 10; val++) {
			val++;
                        socket.write(addr, val);
                }
        }
        SC_CTOR(initiator) {
                SC_THREAD(thread);
        }
};

struct target : sc_module, basic::target_module_base {
        basic::target_socket<target> socket;
        tlm::tlm_response_status write(const basic::addr_t &a,
                                       const basic::data_t &d) {
                cout << name() << " à reçu : " << d << endl;
                return tlm::TLM_OK_RESPONSE;
        }
        tlm::tlm_response_status read (const basic::addr_t &a,
                                       /* */ basic::data_t &d) {
                SC_REPORT_ERROR("TLM", "non implémenté");
                abort();
        }
        SC_CTOR(target) {/* */}
};

int sc_main (int argc, char ** argv) {
        /*
         +---------+     +-------------+    +--------+
         |        +++   +-+           +++  +++       |
         | Alice  | +---+ |  Router   | +--+ |  Bob  |
         |        +++ | +-+           +++  +++       |
         +---------+  |  +-------------+    +--------+
                      |
         +---------+  |
         |        +++ |
         | Lucie  | +-
         |        +++ 
         +---------+ 
         */

        initiator a1("Alice");
        initiator a2("Lucie");
        target b("Bob");
        Bus router("Router");

        /* Bob is mapped at addresses [0, 100[, i.e. ... */
        router.map(b.socket, 0, 100);

        /* connect components to the bus */
        a1.socket.bind(router.target);
        a2.socket.bind(router.target);
        router.initiator.bind(b.socket);

        /* and start simulation */
        sc_start();
        return 0;
}

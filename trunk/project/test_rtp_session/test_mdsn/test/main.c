#include <stdio.h>
#include <stdlib.h>
#include "client.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>




int main()
{
   CLIENT_T* p_Client1=NULL;
   struct sockaddr_in si_other;
	int s_mdns, slen=sizeof(si_other);
    static char buf[BUFLEN];
    size_t actual_size;
    static char message[64];

     p_Client1=ClientNew ();

	/* Prepare Socket */
    if ((s_mdns=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        printf("Error in socket\n");
    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(p_Client1->mdns_port);
	if (inet_aton(p_Client1->mdns_ipv4, &si_other.sin_addr)==0)
	{
        fprintf(stderr, "inet_aton() failed\n");
    }

memset(message,'\0',sizeof(message));
memset(message,0x0b,sizeof(char));
strcat(message,"_apple-midi");
memset(message+12,0x04,sizeof(char));
strcat(message,"_udp");
memset(message+17,0x05,sizeof(char));
strcat(message,"local");

   FillQuery(p_Client1,0,PTR,message);
   FillQueryTxBuff(p_Client1->dp_queries[0],buf,&actual_size);


    if (sendto(s_mdns, buf, (socklen_t)actual_size, 0, (struct sockaddr*)&si_other, slen)==-1)
    			printf("Error in sendto()\n");

   Client_free(p_Client1);
   close(s_mdns);
}

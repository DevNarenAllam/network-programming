/************************************************************************************
tifconfig.c
prints interface details like ifconfig including wirelese extentions
TRDDC - Tata Research Design and Development Center - TCS
Narendra Allam
************************************************************************************/
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <netinet/in.h>
	#include <netinet/in.h>
	#include <linux/wireless.h>
	#include <linux/if.h>
	#include <arpa/inet.h>
	#include <stdio.h>
	#include <string.h>
	
	/*
	* Interface request structure used for socket
	* ioctl's.  All interface ioctl's must have parameter
	* definitions which begin with ifr_name.  The
	* remainder may be interface specific.
	*/
	
	//	struct	ifr {
	//	#define	IFNAMSIZ	16
	//		char	ifr_name[IFNAMSIZ];		/* if name, e.g. "en0" */
	//		union {
	//			struct	sockaddr ifru_addr;
	//			struct	sockaddr ifru_dstaddr;
	//			struct	sockaddr ifru_broadaddr;
	//			short	ifru_flags;
	//			int	ifru_metric;
	//			caddr_t	ifru_data;
	//	} ifr_ifru;
	
	//	#define	ifr_addr	ifr_ifru.ifru_addr	/* address */
	//	#define	ifr_dstaddr	ifr_ifru.ifru_dstaddr	/* other end of p-to-p link */
	//	#define	ifr_broadaddr	ifr_ifru.ifru_broadaddr	/* broadcast address */
	//	#define	ifr_flags	ifr_ifru.ifru_flags	/* flags */
	//	#define	ifr_metric	ifr_ifru.ifru_metric	/* metric */
	//	#define	ifr_data	ifr_ifru.ifru_data	/* for use by interface */
	//	};
	
	//	struct ifaliasreq {
	//		char	ifra_name[IFNAMSIZ];		/* if name, e.g. "en0" */
	//		struct	sockaddr ifra_addr;
	//		struct	sockaddr ifra_broadaddr;
	//		struct	sockaddr ifra_mask;
	//	};
	
	/*
	* Structure used in SIOCGIFCONF request.
	* Used to retrieve interface configuration
	* for machine (useful for programs which
	* must know all networks accessible).
	*/
	
	//	struct	ifconf {
	//		int	ifc_len;		/* size of associated buffer */
	//		union {
	//			caddr_t	ifcu_buf;
	//			struct	ifr *ifcu_req;
	//			} ifc_ifcu;
	//	#define	ifc_buf	ifc_ifcu.ifcu_buf	/* buffer address */
	//	#define	ifc_req	ifc_ifcu.ifcu_req	/* array of structures returned */
	//  };
	/*
	SIOCGIFCONF
	      Return  a  list  of  interface (transport layer) addresses. This
	      currently means only addresses of the AF_INET (IPv4) family  for
	      compatibility.   The  user passes a ifconf structure as argument
	      to the ioctl. It contains a pointer to an array of ifreq	struc-
	      tures in ifc_req and its length in bytes in ifc_len.  The kernel
	      fills the ifreqs with all current L3  interface  addresses  that
	      are running: ifr_name contains the interface name (eth0:1 etc.),
	      ifr_addr the address.  The kernel returns with the actual length
	      in  ifc_len.   If  ifc_len  is  equal to the original length the
	      buffer probably has overflowed and you should retry with a  big-
	      ger buffer to get all addresses.	When no error occurs the ioctl
	      returns 0; otherwise -1. Overflow is not an error.
	*/
/*
 * The structure to exchange data for ioctl.
 * This structure is the same as 'struct ifreq', but (re)defined for
 * convenience...
 * Do I need to remind you about structure size (32 octets) ?
 */
// <linux/wireless.h>
//struct	iwreq 
//{
//	union
//	{
//		char	ifrn_name[IFNAMSIZ];	/* if name, e.g. "eth0" */
//	} ifr_ifrn;
//
//	/* Data part (defined just above) */
//	union	iwreq_data	u;
//};
 
	int main(int argc,char*argv[])
	{
		struct ifconf ifcon;
		int sd;
		int i;
		int nif_count=0;
		char buf[1000000];

		sd = socket(AF_INET, SOCK_DGRAM, 0);
	
		ifcon.ifc_len = sizeof (buf);
		ifcon.ifc_buf = buf;
		/*List of interfaces*/
		ioctl(sd,SIOCGIFCONF,&ifcon);
		nif_count = ifcon.ifc_len / sizeof(struct ifreq);
	printf("\n--------------------------------------\n");	
	printf("Custom ifconfig : ifconfig + iwconfig");
	printf("\n--------------------------------------\n");	
	for(i=0;i<nif_count;i++)
		{
			printf("Interface name: %s\n",ifcon.ifc_req[i].ifr_name);
			ip_details(ifcon.ifc_req[i].ifr_name);
			printf("\n");
		}
		close(sd);

	return 0;
	}

int ip_details(const char *ifname)
{
	int fd;
	struct ifreq ifreq;
	struct iwreq wrq;  //wireless structure
 
    unsigned char *hw;
    struct sockaddr_in *sin;
    char *itf;

    char hwaddr[40];
    char addr[20];
    char brdaddr[20];
    char netmask[20];

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    /* hardware address */
	strcpy(ifreq.ifr_name,ifname);

	ioctl(fd, SIOCGIFHWADDR, &ifreq);
    hw = ifreq.ifr_hwaddr.sa_data;
//To Do : Here we have to consider Network byte order,this logic is for Littel Endian
// For Big-Endian boards it  has to be replaced with the following line
//sprintf(hwaddr, "%02x:%02x:%02x:%02x:%02x:%02x",*(hw + 5), *(hw + 4), *(hw + 3), *(hw + 2), *(hw + 1), *hw);

    sprintf(hwaddr, "%02x:%02x:%02x:%02x:%02x:%02x",*hw, *(hw + 1), *(hw + 2), *(hw + 3), *(hw + 4), *(hw + 5));
	printf("MAC address : %s \n",hwaddr);
	
    /* address */
    ioctl(fd, SIOCGIFADDR, &ifreq);
	sin = (struct sockaddr_in *)&ifreq.ifr_broadaddr; 
	printf("IP address  %s\n",inet_ntoa(sin->sin_addr));

    /* broadcast */
    ioctl(fd, SIOCGIFBRDADDR, &ifreq);
    sin = (struct sockaddr_in *)&ifreq.ifr_broadaddr; 
	 printf("Broadcast Addr %s\n",inet_ntoa(sin->sin_addr));
	
	

    /* netmask */
    ioctl(fd, SIOCGIFNETMASK, &ifreq);
    sin = (struct sockaddr_in *)&ifreq.ifr_broadaddr; 
	printf("netmask  %s\n",inet_ntoa(sin->sin_addr));
	
   /*Checking Wireless extention*/

 	/* Get wireless name */  
	strcpy(wrq.ifr_name, ifname);  
	if(ioctl(fd, SIOCGIWNAME, &wrq) < 0)
	{
		printf("No wireless Extention\n");
	}  
	else
	{
		printf("Wireless device\n\n");
	}
	
	return 0;
}

void inet_str(long unsigned addr)
{
long unsigned taddr;
taddr=addr;
printf("%d:",(taddr << 24)>>24);
taddr=addr;
printf("%d:",(taddr << 16)>>24);
taddr=addr;
printf("%d:",(taddr << 8)>>24);
taddr=addr;
printf("%d",taddr >> 24);

}

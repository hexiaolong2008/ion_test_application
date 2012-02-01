#include <stdio.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#include <linux/ion.h>

#define ION_DEVICE "/dev/ion" 
#define MAX_BUFFERS 10

int main(int argc, char ** argv)
{
	int ion_fd = 0;
	int err = 0;
	int heap = 0;
	int i = 0;

	struct ion_allocation_data handle_data[MAX_BUFFERS];
	struct ion_fd_data fd_data[MAX_BUFFERS];
	void *buffer[MAX_BUFFERS];
	
	printf("Open ION memory manager\n");
	ion_fd = open(ION_DEVICE, O_RDONLY);
	if(!ion_fd) {
		printf("Failed to opn ion device - %s", strerror(errno));
		return -errno;
	}	
	
	if(argc == 2) {
		heap = atoi(argv[1]);
	}

	for (i=0; i < MAX_BUFFERS; i++) {

		/* Fill ioctl data to get a buffer */	
		handle_data[i].len = 4096;	
		handle_data[i].align = 0;
		/* get a buffer from cma_zone_x */
		handle_data[i].flags = 1 << heap;
	
		printf("request buffer allocation from heap %d\n", heap);
		err = ioctl(ion_fd, ION_IOC_ALLOC, &handle_data[i]);
		if(err) {
			printf("ION_IOC_ALLOC failed - %s\n", strerror(errno));
			goto out;
		}

		/* now get fd from handle */
		fd_data[i].handle = handle_data[i].handle;

		printf("get fd from handle %x\n", fd_data[i].handle);
		err = ioctl(ion_fd, ION_IOC_SHARE, &fd_data[i]);
		if(err) {
			printf("ION_IOC_SHARE failed - %s\n", strerror(errno));
			goto err;
		}	

		printf("mmap fd %d\n", fd_data[i].fd);
		buffer[i] = mmap(0, handle_data[i].len, PROT_READ|PROT_WRITE, MAP_SHARED, fd_data[i].fd, 0);

		if(buffer[i] == MAP_FAILED) {
			printf("mmap failed - %s\n", strerror(errno));
			err = -errno;
			goto err;
		}

		memset(buffer[i], 0xFF, handle_data[i].len);

		printf("ION test OK heap %d handle %x fd %d\n", heap, handle_data[i].handle, fd_data[i].fd);
		sleep(2);
	}
	printf("wait 10 seconds before release the buffers\n");
	sleep(10);
err:
	for ( ; i > 0; i--) {
		printf("release buffer\n");
		ioctl(ion_fd, ION_IOC_FREE, &handle_data[i]);
	}
out:
	printf("close %s\n", ION_DEVICE);
	close(ion_fd);
	return err;
}

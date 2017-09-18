#include <stdio.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

#include <ion.h>

#define ION_DEVICE "/dev/ion"
#define MAX_HEAPS 8
#define MAX_LEN 1024*16

int main(int argc, char ** argv)
{
	int ion_fd = 0;
	int err, i;
	struct ion_heap_data heaps_data[MAX_HEAPS];
	struct ion_heap_query query;

	printf("Open ION memory manager\n");
	ion_fd = open(ION_DEVICE, O_RDONLY);
	if(!ion_fd) {
		printf("Failed to open ION device - %s\n", strerror(errno));
		return -errno;
	}

	memset(&query, 0, sizeof(query));
	query.cnt = MAX_HEAPS;
	query.heaps = (__u64)&heaps_data;
	err = ioctl(ion_fd, ION_IOC_HEAP_QUERY, &query);
	if (err) {
		printf("Can't get ION heaps data %s\n", strerror(errno));
		goto out;
	}

	printf("Heap name \t\ttype \tid \talloc \tmmap\n");
	for (i = 0; i < query.cnt; i++) {
		int fd;
		struct ion_allocation_data alloc;
		void *buffer = NULL;

		memset(&alloc, 0, sizeof(alloc));
		alloc.len = MAX_LEN;
		alloc.heap_id_mask = heaps_data[i].heap_id;
		err = ioctl(ion_fd, ION_IOC_ALLOC, &alloc);
		if (!err)
			buffer = mmap(0, MAX_LEN, PROT_READ|PROT_WRITE, MAP_SHARED, alloc.fd, 0);

		printf("%s \t\t%d \t%d \t%s \t%s\n",
			heaps_data[i].name,
			heaps_data[i].type ,
			heaps_data[i].heap_id,
			err ? "KO" : "OK",
			buffer ? "OK": "KO");
		close(fd);
	}

out:
	printf("close %s\n", ION_DEVICE);
	close(ion_fd);
	return err;
}

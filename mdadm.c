#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include "mdadm.h"
#include "jbod.h"
#include <stdlib.h>

// function to get the operation code
uint32_t getOperationCode(jbod_cmd_t jbod_op, int DN, int BN)
{
	uint32_t op = jbod_op << 26 | DN << 22 | BN;
	return op;
}

bool mounted = false;
int mdadm_mount(void)
{
	int result = jbod_operation(getOperationCode(JBOD_MOUNT, 0, 0), NULL);
	if (result == 0)
	{
		mounted = true;
		return 1;
	}
	return -1;
}
int mdadm_unmount(void)
{
	int result = jbod_operation(getOperationCode(JBOD_UNMOUNT, 0, 0), NULL);
	if (result == 0)
	{
		mounted = false;
		return 1;
	}
	return -1;
}
int seek(int DiskNum, int BlockNum)
{
	uint32_t op = getOperationCode(JBOD_SEEK_TO_DISK, DiskNum, 0);
	uint32_t op_seek = getOperationCode(JBOD_SEEK_TO_BLOCK, 0, BlockNum);
	jbod_operation(op, NULL);
	jbod_operation(op_seek, NULL);
	return 1;
}

// helper functions
int getMin(int n, int n2)
{
	if (n > n2)
		return n2;
	return n;
}
void find_disk_addrs(uint32_t addr, int *DiskNum, int *BlockNum, int *offsetAddr)
{
	int block_remainder = addr % JBOD_DISK_SIZE;
	*offsetAddr = block_remainder % JBOD_BLOCK_SIZE;
	*BlockNum = block_remainder / JBOD_BLOCK_SIZE;
	*DiskNum = addr / JBOD_DISK_SIZE;
}

int mdadm_read(uint32_t addr, uint32_t length, uint8_t *buffer)
{
	if (length > 1024 || mounted == false || addr + length >= JBOD_NUM_DISKS * JBOD_DISK_SIZE || (length && buffer == NULL))
		return -1;
	uint8_t buffer_copy[256];
	int final_total = length, ReadData = 0, currentAddr = addr, sum = 0, iter = 0;
	do
	{
		int DiskNum, BlockNum, offsetAddr;
		find_disk_addrs(currentAddr, &DiskNum, &BlockNum, &offsetAddr);
		seek(DiskNum, BlockNum);
		jbod_operation(getOperationCode(JBOD_READ_BLOCK, 0, 0), buffer_copy);
		if (iter == 0)
		{
			memcpy(buffer + sum, buffer_copy + offsetAddr, getMin((JBOD_BLOCK_SIZE - offsetAddr), length));
			ReadData = getMin((JBOD_BLOCK_SIZE - offsetAddr), length);
			iter++;
		}
		else
		{
			if (final_total < JBOD_BLOCK_SIZE)
			{
				memcpy(buffer + sum, buffer_copy, final_total);
				ReadData = final_total;
			}
			else
			{
				memcpy(buffer + sum, buffer_copy, JBOD_BLOCK_SIZE);
				ReadData = JBOD_BLOCK_SIZE;
			}
		}
		sum += ReadData;
		final_total -= ReadData;
		currentAddr += ReadData;
	} while (currentAddr < addr + length);
	return length;
}

int mdadm_write(uint32_t addr, uint32_t length, const uint8_t *buf)
{
	if (length > 1024 || mounted == false || addr + length >= JBOD_NUM_DISKS * JBOD_DISK_SIZE || (length && buf == NULL))
		return -1;
	int final_total = length,ReadData = 0,currentAddr = addr,sum = 0,iter = 0;
	uint8_t buffer_copy[256];
	do
	{
		int disk_num, block_num, offset;
		find_disk_addrs(currentAddr, &disk_num, &block_num, &offset);
		seek(disk_num, block_num);
		jbod_operation(getOperationCode(JBOD_READ_BLOCK, 0, 0), buffer_copy);
		if (iter == 0)
		{
			int min_val = getMin((JBOD_BLOCK_SIZE - offset), length);
			memcpy(buffer_copy + offset, buf + sum, min_val);
			seek(disk_num, block_num);
			jbod_operation(getOperationCode(JBOD_WRITE_BLOCK, 0, 0), buffer_copy);
			ReadData = min_val;
			iter += 1;
		}
		else
		{
			if (final_total < JBOD_BLOCK_SIZE)
			{
				memcpy(buffer_copy, buf + sum, final_total);
				seek(disk_num, block_num);
				jbod_operation(getOperationCode(JBOD_WRITE_BLOCK, 0, 0), buffer_copy);
				ReadData = final_total;
			}
			else
			{
				memcpy(buffer_copy, buf + sum, JBOD_BLOCK_SIZE);
				seek(disk_num, block_num);
				jbod_operation(getOperationCode(JBOD_WRITE_BLOCK, 0, 0), buffer_copy);
				ReadData = JBOD_BLOCK_SIZE;
			}
		}
		sum += ReadData;
		final_total -= ReadData;
		currentAddr += ReadData;
	} while (currentAddr < addr + length);
	return length;
}

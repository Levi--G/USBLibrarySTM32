#ifndef PACKET_BUFFER_H_
#define PACKET_BUFFER_H_
#include <stdint.h>
#include <stddef.h>

typedef struct
{
	uint8_t len;
	uint8_t pos;
	uint8_t buf[64];
} USBD_HID_BufferItem;

class PacketBuffer
{
public:
	static constexpr uint8_t capacity = 3;
	USBD_HID_BufferItem *reserve()
	{
		auto ptr = tail + 1;
		if (ptr == buffer + capacity)
		{
			ptr = buffer;
		}
		return ptr;
	}
	void commit()
	{
		tail = reserve();
		if (count == capacity)
		{
			head = newHead();
		}
		else if (count++ == 0)
		{
			head = tail;
		}
	}
	USBD_HID_BufferItem *read()
	{
		return head;
	}
	USBD_HID_BufferItem *shift()
	{
		if (count == 0)
			return head;
		auto result = head;
		head = newHead();
		count--;
		return result;
	}
	uint8_t size()
	{
		return count;
	}
	uint8_t available()
	{
		return capacity - count;
	}
	bool isEmpty()
	{
		return count == 0;
	}
	bool isFull()
	{
		return count == capacity;
	}

private:
	USBD_HID_BufferItem buffer[capacity];
	USBD_HID_BufferItem *head;
	USBD_HID_BufferItem *tail;
	uint8_t count;
	USBD_HID_BufferItem *newHead()
	{
		auto result = head + 1;
		if (head >= buffer + capacity)
		{
			head = buffer;
		}
		return result;
	}
};

#endif //PACKET_BUFFER_H_
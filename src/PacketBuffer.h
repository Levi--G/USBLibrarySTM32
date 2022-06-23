#ifndef PACKET_BUFFER_H_
#define PACKET_BUFFER_H_
#include <stdint.h>
#include <stddef.h>

#define PACKETBUFFER_SIZE 64
#define PACKETBUFFER_COUNT 3

typedef struct
{
	uint8_t len;
	uint8_t pos;
	uint8_t buf[PACKETBUFFER_SIZE];

	uint8_t Read(uint8_t *data, uint8_t length)
	{
		uint8_t read = min(Remaining(), length);
		if (read)
		{
			memcpy(data, buf + pos, read);
			pos += read;
		}
		return read;
	}
	uint8_t Remaining()
	{
		return len - pos;
	}
	bool Empty()
	{
		return Remaining() <= 0;
	}
} USBD_HID_BufferItem;

class PacketBuffer
{
public:
	static constexpr uint8_t capacity = PACKETBUFFER_COUNT;
	USBD_HID_BufferItem *reserve()
	{
		auto result = newTail();
		if (count == capacity)
		{
			shift();
		}
		return result;
	}
	void commit()
	{
		tail = newTail();
		if (count == capacity)
		{
			//should not happen but you never know
			shift();
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
		if (count <= 0)
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
	void clear()
	{
		count = 0;
		head = buffer;
		tail = buffer;
	}
	uint16_t totalBytesAvailable()
	{
		uint16_t total = 0;
		auto ptr = read();
		for (size_t i = 0; i < size(); i++)
		{
			total += ptr->Remaining();
			ptr = nextPtr(ptr);
		}
		return total;
	}

private:
	USBD_HID_BufferItem buffer[capacity];
	USBD_HID_BufferItem *head = buffer;
	USBD_HID_BufferItem *tail = buffer;
	uint8_t count = 0;
	USBD_HID_BufferItem *newHead()
	{
		return nextPtr(head);
	}
	USBD_HID_BufferItem *newTail()
	{
		return nextPtr(tail);
	}
	USBD_HID_BufferItem *nextPtr(USBD_HID_BufferItem *current)
	{
		auto result = current + 1;
		if (result >= buffer + capacity)
		{
			result = buffer;
		}
		return result;
	}
};

#endif //PACKET_BUFFER_H_
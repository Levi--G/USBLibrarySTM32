#ifndef PACKET_BUFFER_H_
#define PACKET_BUFFER_H_
#include <stdint.h>
#include <stddef.h>

#ifndef EP0_PACKETBUFFER_COUNT
#define EP0_PACKETBUFFER_COUNT 3
#endif

// Packetbuffer count determines the buffer size, this can be lowered to save ram, but more missed data may occur
#ifndef PACKETBUFFER_COUNT
#define PACKETBUFFER_COUNT 3
#endif

#ifndef PACKETBUFFER_ALLOW_OVERWRITE
#define PACKETBUFFER_ALLOW_OVERWRITE true
#endif

template <int size>
struct USBD_HID_BufferItem
{
	uint8_t len = 0;
	uint8_t pos = 0;
	uint8_t buf[size];

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
	void Clear()
	{
		len = 0;
		pos = 0;
	}
};

template <int buffersize, int capacity>
class PacketBuffer
{
public:
	USBD_HID_BufferItem<buffersize> *reserve()
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
			// should not happen but you never know
			shift();
		}
		else if (count++ == 0)
		{
			head = tail;
		}
	}
	USBD_HID_BufferItem<buffersize> *read()
	{
		return head;
	}
	USBD_HID_BufferItem<buffersize> *shift()
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
		for (size_t i = 0; i < capacity; i++)
		{
			buffer[i].Clear();
		}
	}
	uint16_t totalBytesAvailable()
	{
		uint16_t total = 0;
		auto ptr = read();
		for (size_t i = 0; i < count; i++)
		{
			total += ptr->Remaining();
			ptr = nextPtr(ptr);
		}
		return total;
	}
	uint16_t nextPacketSize()
	{
		return read()->Remaining();
	}

private:
	USBD_HID_BufferItem<buffersize> buffer[capacity];
	USBD_HID_BufferItem<buffersize> *head = buffer;
	USBD_HID_BufferItem<buffersize> *tail = buffer;
	uint8_t count = 0;
	USBD_HID_BufferItem<buffersize> *newHead()
	{
		return nextPtr(head);
	}
	USBD_HID_BufferItem<buffersize> *newTail()
	{
		return nextPtr(tail);
	}
	USBD_HID_BufferItem<buffersize> *nextPtr(USBD_HID_BufferItem<buffersize> *current)
	{
		auto result = current + 1;
		if (result >= buffer + capacity)
		{
			result = buffer;
		}
		return result;
	}
};

#endif // PACKET_BUFFER_H_
#include <iostream>
#include <string>

template <typename T>
class Queue
{
private:
    static const int MAX_SIZE = 100;
    T data[MAX_SIZE];
    int front;
    int rear;

public:
    Queue() : front(-1), rear(-1) {}

    bool isEmpty() const
    {
        return front == -1 && rear == -1;
    }

    bool isFull() const
    {
        return (rear + 1) % MAX_SIZE == front;
    }

    void enqueue(const T &item)
    {
        if (isFull())
        {
            std::cout << "Queue is full. Cannot enqueue." << std::endl;
            return;
        }

        if (isEmpty())
        {
            front = rear = 0;
        }
        else
        {
            rear = (rear + 1) % MAX_SIZE;
        }

        data[rear] = item;
    }

    void dequeue()
    {
        if (isEmpty())
        {
            std::cout << "Queue is empty. Cannot dequeue." << std::endl;
            return;
        }

        if (front == rear)
        {
            front = rear = -1;
        }
        else
        {
            front = (front + 1) % MAX_SIZE;
        }
    }

    T frontValue() const
    {
        if (isEmpty())
        {
            std::cerr << "Queue is empty. No front value." << std::endl;
        }

        return data[front];
    }

    void updateItem(const T &oldValue, const T &newValue)
    {
        if (isEmpty())
        {
            std::cerr << "Queue is empty. Cannot update item." << std::endl;
            return;
        }

        int current = front;

        while (current != (rear + 1) % MAX_SIZE)
        {
            if (data[current] == oldValue)
            {
                data[current] = newValue;
                std::cout << "Item updated successfully." << std::endl;
                return;
            }

            current = (current + 1) % MAX_SIZE;
        }

        std::cerr << "Item not found in the queue." << std::endl;
    }

    void display() const {}
};

// Queue
struct PasienAntrianPendaftaran
{
    std::string nama;
    std::string pilihPoliklinik;
    bool isOnline;
    bool isDequeue;

    // operasi periksa apakah sama
    bool operator==(const PasienAntrianPendaftaran &other) const
    {
        return nama == other.nama && isOnline == other.isOnline && pilihPoliklinik == other.pilihPoliklinik;
    }
};

struct pasienQueue
{
    std::string pilihanPoliklinik, namaAwal, dokter;
    int nomorPasien;
    Pasien dataPasien;
    bool isOnline, isDequeue, isDequeue2;
    bool operator==(const pasienQueue &other) const
    {
        return (namaAwal == other.namaAwal || nomorPasien == other.nomorPasien) && isOnline == other.isOnline && pilihanPoliklinik == other.pilihanPoliklinik;
    }
};

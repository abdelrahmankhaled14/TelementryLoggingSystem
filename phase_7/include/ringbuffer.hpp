#pragma once
#include <vector>
#include <optional>
#include <utility>

template <typename T>
class RingBuffer
{
private:
    std::vector<std::optional<T>> buffer;
    size_t capacity;
    size_t head;      
    size_t count;     

public:
    explicit RingBuffer(size_t cap = 100) 
        : capacity(cap), head(0), count(0)
    {
        buffer.resize(capacity);
    }

    ~RingBuffer() = default;

    RingBuffer(const RingBuffer&) = delete;
    RingBuffer& operator=(const RingBuffer&) = delete;

    RingBuffer(RingBuffer&& other) noexcept
        : buffer(std::move(other.buffer)), 
          capacity(other.capacity), 
          head(other.head), 
          count(other.count)
    {
        other.head = 0;
        other.count = 0;
    }

    RingBuffer& operator=(RingBuffer&& other) noexcept
    {
        if (this != &other) {
            buffer = std::move(other.buffer);
            capacity = other.capacity;
            head = other.head;
            count = other.count;

            other.head = 0;
            other.count = 0;
        }
        return *this;
    }


    bool tryPush(T value)
    {
        bool wasFull = (count >= capacity);
        
        buffer[head] = std::move(value);
        head = (head + 1) % capacity;
        
        if (!wasFull) {
            count++;
            return true; 
        }
        
        return false;
    }

    
    std::optional<T> tryPop()
    {
        if (count == 0) {
            return std::nullopt;  
        }

        size_t tail = (head - count + capacity) % capacity;
        
        std::optional<T> result = std::move(buffer[tail]);
        buffer[tail].reset();
        count--;
        
        return result;
    }

    size_t getCount() const
    {
        return count;
    }

    bool isEmpty() const
    {
        return count == 0;
    }

    bool isFull() const
    {
        return count >= capacity;
    }

    size_t getCapacity() const
    {
        return capacity;
    }

    void clear()
    {
        buffer.clear();
        buffer.resize(capacity);
        head = 0;
        count = 0;
    }
};

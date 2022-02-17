#ifndef MOVE_H
#define MOVE_H
#include <vector>

template <typename T>
class signal_group;

template <typename T>
class signalable {
        signal_group<T>* group;
        friend class signal_group<T>;
public:
        signalable(): group(nullptr) {}

        virtual void handleSignal(int type, void* value) = 0;

        void sendSignal(int type, void* value) {
                group->sendSignal(type, value);
        }
};

template <typename T>
class signal_group: private std::vector<T*> {
public:
        void sendSignal(int type, void* value) {
                for(auto i : *this) {
                        i->handleSignal(type, value);
                }
        }

        T* operator[](int id) {
                return std::vector<T*>::operator[](id);
        }

        T* add_item(T* val) {
                if(val->signalable<T>::group) throw;
                val->signalable<T>::group = this;
                std::vector<T*>::push_back(val);
                return val;
        }
};

#endif // MOVE_H

#pragma once

#include <libutils/Callback.h>
#include <libutils/OwnPtr.h>
#include <libutils/Vector.h>

template <typename T>
class Observable
{
private:
    __noncopyable(Observable);
    __nonmovable(Observable);

    struct Observer
    {
        Observable<T> *_observable;
        Callback<void(T &)> _callback;

        Observer(Observable<T> *observable, Callback<void(T &)> callback)
            : _observable(observable),
              _callback(callback)
        {
            _observable->register_observer(this);
        }
        ~Observer()
        {
            if (_observable)
                _observable->unregister_observer(this);
        }

        void detach()
        {
            _observable = nullptr;
        }

        void notify(T &subject)
        {
            _callback(subject);
        }
    };

    Vector<Observer *> _observers{};

    void register_observer(Observer *ob)
    {
        if (!_observers.contains(ob))
        {
            _observers.push_back(ob);
        }
    }

    void unregister_observer(Observer *ob)
    {
        _observers.remove_value(ob);
    }

public:
    Observable()
    {
    }

    ~Observable()
    {
        for (size_t i = 0; i < _observers.count(); i++)
        {
            _observers[i]->detach();
        }
    }

    void did_update()
    {
        Vector<Observer *> observers_copy{_observers};

        for (size_t i = 0; i < observers_copy.count(); i++)
        {
            observers_copy[i]->notify(*static_cast<T *>(this));
        }
    }

    OwnPtr<Observer> observe(Callback<void(T &)> callback)
    {
        return own<Observer>(this, callback);
    }
};

template <typename T>
using Observer = Observable<T>::Observer;

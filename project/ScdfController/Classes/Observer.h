//
//  Observer.h
//  ScdfController
//
//  Created by Andrea Scuderi on 21/12/14.
//
//

#ifndef ScdfController_Observer_h
#define ScdfController_Observer_h

namespace ScdfCtrl
{
    enum SCDFC_EVENTS
    {
        SCDFC_EVENTS_Update,
        SCDFC_EVENTS_Add_Item,
        SCDFC_EVENTS_Select_Item,
        SCDFC_EVENTS_Remove_Item,
        SCDFC_EVENTS_Move_Item
    };
    
    class SubjectSimple;
    class ObserverSimple {
    public:
        virtual ~ ObserverSimple() {};
        virtual void Update(SubjectSimple* theChangedSubject, SCDFC_EVENTS event) = 0;
    protected:
        ObserverSimple() {};
    };

    class SubjectSimple {
    public:
        virtual ~SubjectSimple() {};
        
        virtual std::list<ObserverSimple*> &GetObservers()
        {
            return _observers;
        }
        
        virtual void Attach(ObserverSimple* o)
        {
            GetObservers().push_back(o);
        }
        
        virtual void Detach(ObserverSimple* o)
        {
            GetObservers().remove(o);
        }
        
        virtual void Notify()
        {
            std::list<ObserverSimple*> &theList=GetObservers();
            std::list<ObserverSimple*>::iterator endIterator=theList.end();
            for (std::list<ObserverSimple*>::iterator i=theList.begin(); i!=endIterator; ++i)
                (*i)->Update(this, SCDFC_EVENTS_Update);
        }
        virtual void NotifyEvent(SCDFC_EVENTS event)
        {
            std::list<ObserverSimple*> &theList=GetObservers();
            std::list<ObserverSimple*>::iterator endIterator=theList.end();
            for (std::list<ObserverSimple*>::iterator i=theList.begin(); i!=endIterator; ++i)
                (*i)->Update(this, event);
        }
        SubjectSimple() {};
    private:
        std::list<ObserverSimple *> _observers;
    };
}

#endif

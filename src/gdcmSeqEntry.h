// gdcmSeqEntry.h

#ifndef GDCMSQDOCENTRY_H
#define GDCMSQDOCENTRY_H

//-----------------------------------------------------------------------------
typedef std::list<gdcmSQItem *> ListSQItem;
//-----------------------------------------------------------------------------

class GDCM_EXPORT gdcmSeqEntry : public gdcmDocEntry 
{
public:
   gdcmSeqEntry(void);
   ~gdcmSeqEntry(void);
   
   virtual void Print(std::ostream &os = std::cout); 

 /// \brief   returns the SQITEM chained List for this SeQuence.
   inline ListSQItem &GetSQItems() 
      {return items;};

 /// \brief   adds the passed ITEM to the ITEM chained List for this SeQuence.      
   inline void AddSQItem(gdcmSQItem *it) 
      {items.push_back(it);};   

 /// \brief   creates a new SQITEM for this SeQuence.
    gdcmSQItem * NewItem(void);
      
protected:

private:

// Variables

/// \brief chained list of SQ Items
   ListSQItem items;

};


//-----------------------------------------------------------------------------
#endif


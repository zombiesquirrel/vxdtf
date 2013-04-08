


#include "tracking/modules/trasan/TSegmentCurl.h"
#include "trg/cdc/TRGCDC.h"
#include "trg/cdc/Layer.h"
#ifdef TRASAN_DEBUG
#include "tracking/modules/trasan/TDebugUtilities.h"
#endif

namespace Belle {

#define inline

  bool TSegmentCurl::ms_smallcell(false);
  bool TSegmentCurl::ms_superb(false);

  TSegmentCurl::TSegmentCurl(const unsigned superLayerId,
                             const unsigned max)
    : m_flagOfUpdate(true), m_MaxLocalLayerId(max), m_superLayerId(superLayerId)
  {
    m_list.removeAll();
//   m_seqOfLayer = new unsigned [max+1];
//   m_sizeOfLayer = new unsigned [max+1];
//   m_layer = new AList<TLink> [max+1];

//   for(unsigned i=0;i<max+1;++i){
//     m_seqOfLayer[i] = m_sizeOfLayer[i] = 0;
//     m_layer[i].removeAll();
//   }
    m_li = new LayerInfo [max + 1];
  }


  TSegmentCurl::TSegmentCurl(TSegmentCurl& s)
    : m_flagOfUpdate(s.m_flagOfUpdate), m_list(s.m_list), m_superLayerId(s.m_superLayerId),
      m_maxSeq(s.m_maxSeq), m_layerIdOfMaxSeq(s.m_layerIdOfMaxSeq), m_numOfSeqOneLayer(s.m_numOfSeqOneLayer),
      m_numOfLargeSeqLayer(s.m_numOfLargeSeqLayer)
  {

//   m_seqOfLayer = new unsigned [m_MaxLocalLayerId+1];
//   m_sizeOfLayer = new unsigned [m_MaxLocalLayerId+1];
//   m_layer = new AList<TLink> [m_MaxLocalLayerId+1];
    if (m_MaxLocalLayerId != s.m_MaxLocalLayerId) {
      delete [] m_li;
      m_MaxLocalLayerId = s.m_MaxLocalLayerId;
      m_li = new LayerInfo [m_MaxLocalLayerId + 1];
    }

    for (unsigned i = 0; i <= m_MaxLocalLayerId; ++i) {
      m_li[i].m_seqOfLayer = s.m_li[i].m_seqOfLayer;
      m_li[i].m_sizeOfLayer = s.m_li[i].m_sizeOfLayer;
      m_li[i].m_layer = s.m_li[i].m_layer;
    }
  }


  TSegmentCurl::TSegmentCurl(TSegmentCurl* s)
    : m_flagOfUpdate(s->m_flagOfUpdate), m_list(s->m_list), m_superLayerId(s->m_superLayerId),
      m_maxSeq(s->m_maxSeq), m_layerIdOfMaxSeq(s->m_layerIdOfMaxSeq), m_numOfSeqOneLayer(s->m_numOfSeqOneLayer),
      m_numOfLargeSeqLayer(s->m_numOfLargeSeqLayer)
  {
//   m_seqOfLayer = new unsigned [m_MaxLocalLayerId+1];
//   m_sizeOfLayer = new unsigned [m_MaxLocalLayerId+1];
//   m_layer = new AList<TLink> [m_MaxLocalLayerId+1];
    if (m_MaxLocalLayerId != s->m_MaxLocalLayerId) {
      delete [] m_li;
      m_MaxLocalLayerId = s->m_MaxLocalLayerId;
      m_li = new LayerInfo [m_MaxLocalLayerId + 1];
    }

    for (unsigned i = 0; i <= m_MaxLocalLayerId; ++i) {
      m_li[i].m_seqOfLayer = s->m_li[i].m_seqOfLayer;
      m_li[i].m_sizeOfLayer = s->m_li[i].m_sizeOfLayer;
      m_li[i].m_layer = s->m_li[i].m_layer;
    }
  }


  TSegmentCurl::~TSegmentCurl(void)
  {
    m_list.removeAll();
//   for(unsigned i=0;i<=m_MaxLocalLayerId;++i){
//     m_layer[i].removeAll();
//   }
//   delete [] m_seqOfLayer;
//   delete [] m_sizeOfLayer;
//   delete [] m_layer;
    delete [] m_li;
  }


  inline
  const unsigned
  TSegmentCurl::maxLocalLayerId(void) const
  {
    return m_MaxLocalLayerId;
  }


  inline
  const unsigned
  TSegmentCurl::superLayerId(void) const
  {
    return m_superLayerId;
  }


  inline
  const unsigned
  TSegmentCurl::seqOfLayer(const unsigned i)
  {
    if (m_flagOfUpdate)update();
    return m_li[i].m_seqOfLayer;
  }


  inline
  const unsigned
  TSegmentCurl::sizeOfLayer(const unsigned i)
  {
    if (m_flagOfUpdate)update();
    return m_li[i].m_sizeOfLayer;
  }


// inline
// const unsigned
// TSegmentCurl::maxLocalLayerId(const unsigned i)
// {
//   m_MaxLocalLayerId = i;
//   return m_MaxLocalLayerId;
// }


// inline
// const unsigned
// TSegmentCurl::superLayerId(const unsigned i)
// {
//   m_superLayerId = i;
//   return m_superLayerId;
// }


  inline
  void
  TSegmentCurl::append(TLink& e)
  {
    m_flagOfUpdate = true;
    m_li[e.hit()->wire().localLayerId()].m_layer.append(e);
    m_list.append(e);
  }


  inline
  void
  TSegmentCurl::append(TLink* e)
  {
    m_flagOfUpdate = true;
    m_li[e->hit()->wire().localLayerId()].m_layer.append(e);
    m_list.append(e);
  }


  inline
  void
  TSegmentCurl::append(AList<TLink> &e)
  {
    m_flagOfUpdate = true;
    for (unsigned i = 0; i < (unsigned) e.length(); ++i)
      m_li[e[i]->hit()->wire().localLayerId()].m_layer.append(e[i]);
    m_list.append(e);
  }


  inline
  void
  TSegmentCurl::remove(TLink& e)
  {
    m_flagOfUpdate = true;
    m_li[e.hit()->wire().localLayerId()].m_layer.remove(e);
    m_list.remove(e);
  }


  inline
  void
  TSegmentCurl::remove(TLink* e)
  {
    m_flagOfUpdate = true;
    m_li[e->hit()->wire().localLayerId()].m_layer.remove(e);
    m_list.remove(e);
  }


  inline
  void
  TSegmentCurl::remove(AList<TLink> &e)
  {
    m_flagOfUpdate = true;
    //  dump();
    for (unsigned i = 0; i < (unsigned) e.length(); ++i) {
      //    e[i]->dump();
      if (e[i]->hit()->wire().superLayerId() == superLayerId()) {
        m_li[e[i]->hit()->wire().localLayerId()].m_layer.remove(e[i]);
      }
    }
    m_list.remove(e);
  }


  inline
  void
  TSegmentCurl::removeAll(void)
  {
    m_flagOfUpdate = true;
    m_list.removeAll();
    for (unsigned i = 0; i <= m_MaxLocalLayerId; ++i) {
      m_li[i].m_layer.removeAll();
    }
  }


  inline
  const unsigned
  TSegmentCurl::maxSeq(void) const
  {
    // I try to use non-const member.
    // If this method is bad, other methods are necessary.
    TSegmentCurl* const localThis = const_cast<TSegmentCurl * const>(this);
    if (m_flagOfUpdate)localThis->update();
    return m_maxSeq;
  }


  inline
  const unsigned
  TSegmentCurl::layerIdOfMaxSeq(void)
  {
    if (m_flagOfUpdate)update();
    return m_layerIdOfMaxSeq;
  }


  inline
  const unsigned
  TSegmentCurl::numOfSeqOneLayer(void)
  {
    if (m_flagOfUpdate)update();
    return m_numOfSeqOneLayer;
  }


  inline
  const unsigned
  TSegmentCurl::numOfLargeSeqLayer(void)
  {
    if (m_flagOfUpdate)update();
    return m_numOfLargeSeqLayer;
  }


  void
  TSegmentCurl::update(void)
  {
    if (m_list.length() == 0) {
      m_flagOfUpdate = false;
      return;
    }
    m_maxSeq = m_layerIdOfMaxSeq = 0;
    m_numOfSeqOneLayer = m_numOfLargeSeqLayer = 0;
    // bug???
    //  for(unsigned i = 0; i <= m_MaxLocalLayerId; ++i){
    // ok...
    for (unsigned i = 0; i <= m_MaxLocalLayerId; ++i) {
      m_li[i].m_sizeOfLayer = m_li[i].m_layer.length();
      calcuSeq(i);
      if (m_li[i].m_seqOfLayer >= m_maxSeq) {
        m_maxSeq = m_li[i].m_seqOfLayer;
        m_layerIdOfMaxSeq = i;
      }
      if (m_li[i].m_seqOfLayer == 1)++m_numOfSeqOneLayer;
      if (m_li[i].m_seqOfLayer >= TCURL_LARGE_SEQUENCE)++m_numOfLargeSeqLayer;
    }

    m_flagOfUpdate = false;
  }


  TSegmentCurl&
  TSegmentCurl::operator=(const TSegmentCurl& s)
  {
    if (this == &s)return *this;

    m_flagOfUpdate = s.m_flagOfUpdate;
    m_list = s.m_list;
    //  m_MaxLocalLayerId = s.m_MaxLocalLayerId;
    m_superLayerId = s.m_superLayerId;

    m_maxSeq = s.m_maxSeq;
    m_layerIdOfMaxSeq = s.m_layerIdOfMaxSeq;
    m_numOfSeqOneLayer = s.m_numOfSeqOneLayer;
    m_numOfLargeSeqLayer = s.m_numOfLargeSeqLayer;

//   delete [] m_seqOfLayer;
//   delete [] m_sizeOfLayer;
//   delete [] m_layer;

//   m_seqOfLayer = new unsigned [m_MaxLocalLayerId+1];
//   m_sizeOfLayer = new unsigned [m_MaxLocalLayerId+1];
//   m_layer = new AList<TLink> [m_MaxLocalLayerId+1];

    if (m_MaxLocalLayerId != s.m_MaxLocalLayerId) {
      delete [] m_li;
      m_MaxLocalLayerId = s.m_MaxLocalLayerId;
      m_li = new LayerInfo [m_MaxLocalLayerId + 1];
    }

    for (unsigned i = 0; i <= m_MaxLocalLayerId; ++i) {
      m_li[i].m_seqOfLayer = s.m_li[i].m_seqOfLayer;
      m_li[i].m_sizeOfLayer = s.m_li[i].m_sizeOfLayer;
      m_li[i].m_layer = s.m_li[i].m_layer;
    }

    return *this;
  }

#if defined(__GNUG__)
  int
  sortByWireSerialNumber(const TLink** a, const TLink** b)
  {
    if ((*a)->hit()->wire().id() < (*b)->hit()->wire().id()) {
      return 1;
    } else if ((*a)->hit()->wire().id() == (*b)->hit()->wire().id()) {
      return 0;
    } else {
      return -1;
    }
  }
#else
  extern "C" int
  sortByWireSerialNumber(const void* av, const void* bv)
  {
    const TLink** a((const TLink**)av);
    const TLink** b((const TLink**)bv);
    if ((*a)->hit()->wire().id() < (*b)->hit()->wire().id()) {
      return 1;
    } else if ((*a)->hit()->wire().id() == (*b)->hit()->wire().id()) {
      return 0;
    } else {
      return -1;
    }
  }
#endif

  void
  TSegmentCurl::calcuSeq(unsigned i)
  {
//cnv  //...exception
//   if(i > m_MaxLocalLayerId){
//     m_li[i].m_seqOfLayer = 0;
//     return;
//   }
//   unsigned size = m_li[i].m_layer.length();
//   if(size < 1){
//     m_li[i].m_seqOfLayer = 0;
//     return;
//   }
//   if(size == 1){
//     m_li[i].m_seqOfLayer = 1;
//     return;
//   }

//   //...initialize
//   m_li[i].m_layer.sort(sortByWireSerialNumber);
//   unsigned seq = 1;
//   unsigned maxSeq = 0;

//   //...calculation
//   if(m_li[i].m_layer[0]->hit()->wire().localId() == static_cast<unsigned>(wires(m_superLayerId)-1) &&
//      m_li[i].m_layer[size-1]->hit()->wire().localId() == 0){
//     for(unsigned j=0;j<size-1;++j){
//       if(m_li[i].m_layer[j]->hit()->wire().localIdForMinus()-1 ==
//   (int) m_li[i].m_layer[j+1]->hit()->wire().localId()){
//  ++seq;
//       }else{
//  break;
//       }
//       if(j == size-2)goto loop;
//     }
//     ++seq;
//     for(unsigned j=size-1;j>0;--j){
//       if(m_li[i].m_layer[j]->hit()->wire().localIdForPlus()+1 ==
//   (int) m_li[i].m_layer[j-1]->hit()->wire().localId()){
//  ++seq;
//       }else{
//  break;
//       }
//     }
//      loop:
//     if(seq > maxSeq)maxSeq = seq;
//   }
//   seq = 1;
//   for(unsigned j=0;j<size-1;++j){
//     if(m_li[i].m_layer[j]->hit()->wire().localIdForMinus()-1 ==
//        (int) m_li[i].m_layer[j+1]->hit()->wire().localId()){
//       ++seq;
//     }else{
//       if(seq > maxSeq)maxSeq = seq;
//       seq = 1;
//     }
//   }
//   if(seq > maxSeq)maxSeq = seq;

//   //...set and return
//   m_li[i].m_seqOfLayer = maxSeq;
//   return;
  }


  unsigned
  TSegmentCurl::wires(const unsigned superLayerId) const
  {
//cnv

//   const TRGCDC &cdc = *TRGCDC::getTRGCDC();
//   const AList<TRGCDCLayer> &sl=*cdc.superLayer(superLayerId);
//   if(ms_superb) {
//     return sl[0]->nWires();
//   } else {

//   // input   - super layer id#(0-10)
//   // output  - # of its super layer
//   if(superLayerId ==  0)return  64;
//   if(superLayerId ==  1)return  80;
//   if(superLayerId ==  2)return  96;
//   if(superLayerId ==  3)return 128;
//   if(superLayerId ==  4)return 144;
//   if(superLayerId ==  5)return 160;
//   if(superLayerId ==  6)return 192;
//   if(superLayerId ==  7)return 208;
//   if(superLayerId ==  8)return 240;
//   if(superLayerId ==  9)return 256;
//   if(superLayerId == 10)return 288;
//   }

//   std::cout << "Error in the SegmentCurl(wires)." << std::endl;


    return 0;
  }


  void
  TSegmentCurl::dump(void)
  {
#ifdef TRASAN_DEBUG_DETAIL
    // std::cout.form(" Hep  idhep mother mcPX
    // mcPY  mcPZ  mcE   mcMass\n");
    // std::cout.form(" %3d   % 4d   % 4d % 2.2f
    // % 2.2f % 2.2f % 2.2f % 2.3f\n",
    // hepID, partID, mothID, mcpx, mcpy, mcpz, mce, mcmass);

    const std::string tab = Tab() + "    ";

    std::cout << tab << "=====Segment for Curling Track Finder====="
              << std::endl;
    std::cout << tab << "List Size = " << m_list.length() << std::endl;
    std::cout << tab << "Flag of Update = " << m_flagOfUpdate
              << ", Max Local Layer ID = " << m_MaxLocalLayerId
              << ", Super Layer ID = " << m_superLayerId << std::endl;
    std::cout << tab << "Max Seq. = " << m_maxSeq << ", Its Layer ID = "
              << m_layerIdOfMaxSeq << std::endl;
    std::cout << tab << "# of Seq = 1 Layer = " << m_numOfSeqOneLayer
              << ", # of Large Seq. Layer = " << m_numOfLargeSeqLayer
              << std::endl;
    // std::cout.form(" ID | Seq. of Layer | Size of Layer | Local ID List\n");
    std::cout << tab << "ID | Seq. of Layer | Size of Layer | Local ID List"
              << std::endl;
    for (unsigned i = 0; i < m_MaxLocalLayerId + 1; ++i) {
      // std::cout.form("  %1d |  %2d           | %2d            |", i,
      // m_seqOfLayer[i], m_sizeOfLayer[i]);
      std::cout << tab << i << " | " << m_li[i].m_seqOfLayer << "   | "
                << m_li[i].m_sizeOfLayer << "   | " << m_li[i].m_layer.length() << "   | ";
      for (unsigned j = 0, size = m_li[i].m_layer.length(); j < size; ++j)
        std::cout << m_li[i].m_layer[j]->hit()->wire().localId()
                  << ", ";
      std::cout << tab << std::endl;
    }
    std::cout << tab << "=====End of Segment====="
              << std::endl;
#endif
  }

} // namespace Belle
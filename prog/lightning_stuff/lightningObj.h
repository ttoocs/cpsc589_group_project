#include "lightning.h"

namespace lightning{

class lightningObj{
  static std::vector<lightningObj*> strikes;
  static std::vector<Segment> getAllSegments();

  std::vector<Segment> Segments;
 
  void genSegments();
  
  lightningObj();

};

}


#include "lightningObj.h"

namespace lightning{

  std::vector<lightningObj*> lightningObj::strikes;

  std::vector<Segment> lightningObj::getAllSegments(){

    std::vector<Segment> r;
    for(auto it=strikes.begin(); it != strikes.end(); it++){
      for(auto is=(*it)->Segments.begin(); is != (*it)->Segments.end(); is++){
        r.push_back(*is);
      }
    }

    return r;
  }

  
  void lightningObj::genSegments(){
    Segments.clear();
    trace_lightning(vec3(0.0,2.0,0.0), vec3(-0.5,-1.0,-0.5), &Segments, 2.0);
  }

  lightningObj::lightningObj(){
    genSegments();
    strikes.push_back(this); 
  }

};

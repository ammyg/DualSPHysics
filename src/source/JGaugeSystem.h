//HEAD_DSPH
/*
 <DUALSPHYSICS>  Copyright (c) 2017 by Dr Jose M. Dominguez et al. (see http://dual.sphysics.org/index.php/developers/). 

 EPHYSLAB Environmental Physics Laboratory, Universidade de Vigo, Ourense, Spain.
 School of Mechanical, Aerospace and Civil Engineering, University of Manchester, Manchester, U.K.

 This file is part of DualSPHysics. 

 DualSPHysics is free software: you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License 
 as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
 
 DualSPHysics is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details. 

 You should have received a copy of the GNU Lesser General Public License along with DualSPHysics. If not, see <http://www.gnu.org/licenses/>. 
*/

//:#############################################################################
//:# Cambios:
//:# =========
//:# - Clase para gestionar la medicion de distintas magnitudes de forma 
//:#   automatica y simple. (12-02-2017)
//:# - Error corregido cargando <default><output>. (03-03-2017)
//:#############################################################################

/// \file JGaugeSystem.h \brief Declares the class \ref JGaugeSystem.

#ifndef _JGaugeSystem_
#define _JGaugeSystem_

#include <string>
#include <vector>
#include "JObject.h"
#include "Types.h"
#include "JGaugeItem.h"

class JXml;
class TiXmlElement;
class JLog2;

//##############################################################################
//# XML format in _FmtXML_Gauges.xml.
//##############################################################################

//##############################################################################
//# JGaugeSystem
//##############################################################################
/// \brief Manages the list of configured gauges.

class JGaugeSystem : protected JObject
{
private:
  JLog2* Log;
  const bool Cpu;

  bool Configured;
  bool Simulate2D;        ///<Toggles 2D simulation (cancels forces in Y axis). | Activa o desactiva simulacion en 2D (anula fuerzas en eje Y).
  double Simulate2DPosY;  ///<Y value in 2D simulations.                        | Valor de Y en simulaciones 2D.
  double TimeMax;
  double TimePart;
  double Dp;              ///<Distance between particles.
  tdouble3 DomPosMin;     ///<Lower limit of simulation + edge 2h if periodic conditions. DomPosMin=Map_PosMin+(DomCelIni*Scell); | Limite inferior de simulacion + borde 2h si hay condiciones periodicas. 
  tdouble3 DomPosMax;     ///<Upper limit of simulation + edge 2h if periodic conditions. DomPosMax=min(Map_PosMax,Map_PosMin+(DomCelFin*Scell)); | Limite inferior de simulacion + borde 2h si hay condiciones periodicas. 
  float Scell;            ///<Cell size: 2h or h. | Tama�o de celda: 2h o h.
  int Hdiv;               ///<Value to divide 2H. | Valor por el que se divide a DosH
  float H;
  float MassFluid;

  JGaugeItem::StDefault CfgDefault; ///<Default configuration.

  std::vector<JGaugeItem*> Gauges;

  //-Variables for GPU.
 #ifdef _WITHGPU
  float3* AuxMemoryg;  ///<Auxiliary allocated memory on GPU [1].
 #endif


  void ResetCfgDefault();
  void LoadLinePoints(double coefdp,const tdouble3 &point1,const tdouble3 &point2,std::vector<tdouble3> &points,const std::string &ref)const;
  void LoadLinePoints(unsigned count,const tdouble3 &point1,const tdouble3 &point2,std::vector<tdouble3> &points,const std::string &ref)const;
  void LoadPoints(JXml *sxml,TiXmlElement* lis,std::vector<tdouble3> &points)const;
  JGaugeItem::StDefault ReadXmlCommon(JXml *sxml,TiXmlElement* ele)const;
  void ReadXml(JXml *sxml,TiXmlElement* ele);
  void SaveVtkInitPoints()const;

public:
  JGaugeSystem(bool cpu,JLog2* log);
  ~JGaugeSystem();
  void Reset();

  void Config(bool simulate2d,double simulate2dposy,double timemax,double timepart
    ,double dp,tdouble3 posmin,tdouble3 posmax,float scell,unsigned hdiv,float h,float massfluid);

  void LoadXml(JXml *sxml,const std::string &place);
  void VisuConfig(std::string txhead,std::string txfoot);

  bool GetSimulate2D()const{ return(Simulate2D); };
  double GetDp()const{ return(Dp); }
  tdouble3 GetDomPosMin()const{ return(DomPosMin); }
  tdouble3 GetDomPosMax()const{ return(DomPosMax); }
  float GetMassFluid()const{ return(MassFluid); }
  float GetH()const{ return(H); }
  float GetScell()const{ return(Scell); }

  void LoadLinePoints(double coefdp,const tdouble3 &point1,const tdouble3 &point2,std::vector<tdouble3> &points)const{ LoadLinePoints(coefdp,point1,point2,points,""); }
  void LoadLinePoints(unsigned count,const tdouble3 &point1,const tdouble3 &point2,std::vector<tdouble3> &points)const{ LoadLinePoints(count,point1,point2,points,""); }

  JGaugeVelocity* AddGaugeVel(std::string name,double computestart,double computeend,double computedt,const tdouble3 &point);
  JGaugeSwl*      AddGaugeSwl(std::string name,double computestart,double computeend,double computedt,tdouble3 point0,tdouble3 point2,double pointdp,float masslimit=0);
  JGaugeMaxZ*     AddGaugeMaxZ(std::string name,double computestart,double computeend,double computedt,tdouble3 point0,double height,float distlimit);

  unsigned GetCount()const{ return(unsigned(Gauges.size())); }
  unsigned GetGaugeIdx(const std::string &name)const;
  JGaugeItem* GetGauge(unsigned c)const;

  void CalculeCpu(double timestep,bool svpart,tuint3 ncells,tuint3 cellmin,const unsigned *begincell
    ,const tdouble3 *pos,const typecode *code,const tfloat4 *velrhop);

 #ifdef _WITHGPU
  void CalculeGpu(double timestep,bool svpart,tuint3 ncells,tuint3 cellmin,const int2 *beginendcell
    ,const double2 *posxy,const double *posz,const typecode *code,const float4 *velrhop);
 #endif

  void SaveResults(unsigned cpart);
};


#endif



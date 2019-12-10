///////////////////////////////////////////////////////////////////////////////////////
//Copyright (C) Yang Zhang Lab, University of Michigan - All Rights Reserved
//Unauthorized copying of this file, via any medium is strictly prohibited
//Written by Xiaoqiang Huang <xiaoqiah@umich.edu>
//////////////////////////////////////////////////////////////////////////////////////

#include "ProgramFunction.h"
#include "RotamerBuilder.h"
#include "RotamerOptimizer.h"
#include <string.h>
#include <ctype.h>

extern BOOL FLAG_MONOMER;
extern BOOL FLAG_PPI;
extern BOOL FLAG_ENZYME;
extern BOOL FLAG_PROT_LIG;

extern BOOL FLAG_ADD_CRYSTAL_ROT;
extern BOOL FLAG_EXPAND_HYDROXYL_ROT;


int NOVA_help(){
  printf(  
    "EvoEF2 program options:\n\n"
    "Basic OPTIONS:\n"
    "Usage: EvoEF [OPTIONS] --pdb=pdbfile\n"  
    "   --version       print version\n"
    "   --help          print help message\n"  
    "   --command=arg   choose your computation type:\n"
    "                   RepairStructure"
    "                   ComputeStability\n"
    "                   ComputeBinding\n"
    "                   BuildMutant\n"
    "                   ProteinDesign\n"
    );
  return Success;
}

int NOVA_version(){
  printf("The energy function engine is EvoEF version 2\n");
  return Success;
}


int NOVA_interface(){
  printf(
    "############################################################################################\n"
    "                                    NOVA                                                    \n"
    "  A framework for macromolecular modeling, e.g.,protein design, protein side-chain packing, \n"
    "protein structure energy minimization, add and optimize hydrogen bonds, build mutant model, \n"
    "calculate protein folding stability, calculate protein-protein binding free energy, etc     \n"
    "\n\n"
    "  Written by Xiaoqiang Huang\n"
    "  (xiaoqiah@umich.edu; tommyhuangthu@foxmail.com)\n"
    "  Copyright (C) The Yang Zhang Lab\n"
    "  Prof. Yang Zhang's Research Group\n"
    "  Dept. of Computational Medicine & Bioinformatics\n"
    "  Medical School\n"
    "  University of Michigan\n"
    "  Ann Arbor, MI 48109-2218, USA\n"
    "  E-mail: zhng@umich.edu\n"
    "############################################################################################\n"
    );
  return Success;
}


///////////////////////////////////////////////////////////////////////////////////////////
//MAIN functions
//////////////////////////////////////////////////////////////////////////////////////////
int NOVA_ComputeChainStability(Structure *pStructure, int chainIndex, double *energyTerms){
  EnergyTermInitialize(energyTerms);
  //ChainComputeResiduePosition(pStructure, chainIndex);
  Chain *pChainI = StructureGetChain(pStructure, chainIndex);
  // interaction between residues within one chain
  for(int ir = 0; ir < ChainGetResidueCount(pChainI); ir++){
    Residue *pResIR = ChainGetResidue(pChainI,ir);
    EVOEF_AminoAcidReferenceEnergy(pResIR->name, energyTerms);
    EVOEF_EnergyResidueIntraEnergy(pResIR,energyTerms);
    for(int is = ir+1; is < ChainGetResidueCount(pChainI); is++){
      Residue *pResIS = ChainGetResidue(pChainI,is);
      if(is==ir+1) EVOEF_EnergyResidueAndNextResidue(pResIR,pResIS,energyTerms);
      else EVOEF_EnergyResidueAndOtherResidueSameChain(pResIR,pResIS,energyTerms);
    }
  }

  EnergyTermWeighting(energyTerms);
  printf("Chain %s energy details:\n", ChainGetName(pChainI));
  printf("reference_ALA         =            %8.2f\n", energyTerms[ 1]);
  printf("reference_CYS         =            %8.2f\n", energyTerms[ 2]);
  printf("reference_ASP         =            %8.2f\n", energyTerms[ 3]);
  printf("reference_GLU         =            %8.2f\n", energyTerms[ 4]);
  printf("reference_PHE         =            %8.2f\n", energyTerms[ 5]);
  printf("reference_GLY         =            %8.2f\n", energyTerms[ 6]);
  printf("reference_HIS         =            %8.2f\n", energyTerms[ 7]);
  printf("reference_ILE         =            %8.2f\n", energyTerms[ 8]);
  printf("reference_LYS         =            %8.2f\n", energyTerms[ 9]);
  printf("reference_LEU         =            %8.2f\n", energyTerms[10]);
  printf("reference_MET         =            %8.2f\n", energyTerms[11]);
  printf("reference_ASN         =            %8.2f\n", energyTerms[12]);
  printf("reference_PRO         =            %8.2f\n", energyTerms[13]);
  printf("reference_GLN         =            %8.2f\n", energyTerms[14]);
  printf("reference_ARG         =            %8.2f\n", energyTerms[15]);
  printf("reference_SER         =            %8.2f\n", energyTerms[16]);
  printf("reference_THR         =            %8.2f\n", energyTerms[17]);
  printf("reference_VAL         =            %8.2f\n", energyTerms[18]);
  printf("reference_TRP         =            %8.2f\n", energyTerms[19]);
  printf("reference_TYR         =            %8.2f\n", energyTerms[20]);
  printf("intraR_vdwatt         =            %8.2f\n", energyTerms[21]);
  printf("intraR_vdwrep         =            %8.2f\n", energyTerms[22]);
  printf("intraR_electr         =            %8.2f\n", energyTerms[23]);
  printf("intraR_deslvP         =            %8.2f\n", energyTerms[24]);
  printf("intraR_deslvH         =            %8.2f\n", energyTerms[25]);
  printf("intraR_hbscbb_dis     =            %8.2f\n", energyTerms[26]);
  printf("intraR_hbscbb_the     =            %8.2f\n", energyTerms[27]);
  printf("intraR_hbscbb_phi     =            %8.2f\n", energyTerms[28]);
  printf("interS_vdwatt         =            %8.2f\n", energyTerms[31]);
  printf("interS_vdwrep         =            %8.2f\n", energyTerms[32]);
  printf("interS_electr         =            %8.2f\n", energyTerms[33]);
  printf("interS_deslvP         =            %8.2f\n", energyTerms[34]);
  printf("interS_deslvH         =            %8.2f\n", energyTerms[35]);
  printf("interS_hbbbbb_dis     =            %8.2f\n", energyTerms[41]);
  printf("interS_hbbbbb_the     =            %8.2f\n", energyTerms[42]);
  printf("interS_hbbbbb_phi     =            %8.2f\n", energyTerms[43]);
  printf("interS_hbscbb_dis     =            %8.2f\n", energyTerms[44]);
  printf("interS_hbscbb_the     =            %8.2f\n", energyTerms[45]);
  printf("interS_hbscbb_phi     =            %8.2f\n", energyTerms[46]);
  printf("interS_hbscsc_dis     =            %8.2f\n", energyTerms[47]);
  printf("interS_hbscsc_the     =            %8.2f\n", energyTerms[48]);
  printf("interS_hbscsc_phi     =            %8.2f\n", energyTerms[49]);
  printf("----------------------------------------------------\n");
  printf("Total                 =            %8.2f\n\n", energyTerms[0]);

  return Success;
}




int NOVA_ComputeStability(Structure *pStructure,AAppTable* pAAppTable,RamaTable* pRama,double energyTerms[MAX_EVOEF_ENERGY_TERM_NUM]){
  //EnergyTermInitialize(energyTerms);
  int aas[20]={0}; //ACDEFGHIKLMNPQRSTVWY, only for regular amino acid
  //StructureGetAminoAcidComposition(pStructure, aas);
  // if the structure is composed of several chains, the residue position could be different in the whole structure from that in the separate chain
  //StructureComputeResiduePosition(pStructure);
  for(int i = 0; i < StructureGetChainCount(pStructure); i++){
    Chain *pChainI = StructureGetChain(pStructure,i);
    for(int ir = 0; ir < ChainGetResidueCount(pChainI); ir++){
      Residue *pResIR = ChainGetResidue(pChainI,ir);
      double refer=0.0;
      EVOEF_AminoAcidReferenceEnergy(pResIR->name, energyTerms);
      EVOEF_EnergyResidueIntraEnergy(pResIR,energyTerms);
      //amino acid propensity and ramachandran energy
      //backbone independent, no dunbrack energy
      AminoAcidPropensityAndRamachandranEnergy(pResIR,pAAppTable,pRama,energyTerms);
      for(int is = ir+1; is < ChainGetResidueCount(pChainI); is++){
        Residue *pResIS = ChainGetResidue(pChainI,is);
        if(is==ir+1) EVOEF_EnergyResidueAndNextResidue(pResIR,pResIS,energyTerms);
        else EVOEF_EnergyResidueAndOtherResidueSameChain(pResIR,pResIS,energyTerms);
      }
      for(int k = i+1; k < StructureGetChainCount(pStructure); k++){
        Chain *pChainK = StructureGetChain(pStructure,k);
        for(int ks = 0; ks < ChainGetResidueCount(pChainK); ks++){
          Residue *pResKS = ChainGetResidue(pChainK,ks);
          EVOEF_EnergyResidueAndOtherResidueDifferentChain(pResIR,pResKS,energyTerms);
        }
      }
    }
  }

  EnergyTermWeighting(energyTerms);
  printf("\nStructure energy details:\n");
  printf("reference_ALA         =            %8.2f\n", energyTerms[ 1]);
  printf("reference_CYS         =            %8.2f\n", energyTerms[ 2]);
  printf("reference_ASP         =            %8.2f\n", energyTerms[ 3]);
  printf("reference_GLU         =            %8.2f\n", energyTerms[ 4]);
  printf("reference_PHE         =            %8.2f\n", energyTerms[ 5]);
  printf("reference_GLY         =            %8.2f\n", energyTerms[ 6]);
  printf("reference_HIS         =            %8.2f\n", energyTerms[ 7]);
  printf("reference_ILE         =            %8.2f\n", energyTerms[ 8]);
  printf("reference_LYS         =            %8.2f\n", energyTerms[ 9]);
  printf("reference_LEU         =            %8.2f\n", energyTerms[10]);
  printf("reference_MET         =            %8.2f\n", energyTerms[11]);
  printf("reference_ASN         =            %8.2f\n", energyTerms[12]);
  printf("reference_PRO         =            %8.2f\n", energyTerms[13]);
  printf("reference_GLN         =            %8.2f\n", energyTerms[14]);
  printf("reference_ARG         =            %8.2f\n", energyTerms[15]);
  printf("reference_SER         =            %8.2f\n", energyTerms[16]);
  printf("reference_THR         =            %8.2f\n", energyTerms[17]);
  printf("reference_VAL         =            %8.2f\n", energyTerms[18]);
  printf("reference_TRP         =            %8.2f\n", energyTerms[19]);
  printf("reference_TYR         =            %8.2f\n", energyTerms[20]);

  printf("intraR_vdwatt         =            %8.2f\n", energyTerms[21]);
  printf("intraR_vdwrep         =            %8.2f\n", energyTerms[22]);
  printf("intraR_electr         =            %8.2f\n", energyTerms[23]);
  printf("intraR_deslvP         =            %8.2f\n", energyTerms[24]);
  printf("intraR_deslvH         =            %8.2f\n", energyTerms[25]);
  printf("intraR_hbscbb_dis     =            %8.2f\n", energyTerms[26]);
  printf("intraR_hbscbb_the     =            %8.2f\n", energyTerms[27]);
  printf("intraR_hbscbb_phi     =            %8.2f\n", energyTerms[28]);
  printf("aapropensity          =            %8.2f\n", energyTerms[91]);
  printf("ramachandran          =            %8.2f\n", energyTerms[92]);
  printf("dunbrack              =            %8.2f\n", energyTerms[93]);

  printf("interS_vdwatt         =            %8.2f\n", energyTerms[31]);
  printf("interS_vdwrep         =            %8.2f\n", energyTerms[32]);
  printf("interS_electr         =            %8.2f\n", energyTerms[33]);
  printf("interS_deslvP         =            %8.2f\n", energyTerms[34]);
  printf("interS_deslvH         =            %8.2f\n", energyTerms[35]);
  printf("interS_ssbond         =            %8.2f\n", energyTerms[36]);
  printf("interS_hbbbbb_dis     =            %8.2f\n", energyTerms[41]);
  printf("interS_hbbbbb_the     =            %8.2f\n", energyTerms[42]);
  printf("interS_hbbbbb_phi     =            %8.2f\n", energyTerms[43]);
  printf("interS_hbscbb_dis     =            %8.2f\n", energyTerms[44]);
  printf("interS_hbscbb_the     =            %8.2f\n", energyTerms[45]);
  printf("interS_hbscbb_phi     =            %8.2f\n", energyTerms[46]);
  printf("interS_hbscsc_dis     =            %8.2f\n", energyTerms[47]);
  printf("interS_hbscsc_the     =            %8.2f\n", energyTerms[48]);
  printf("interS_hbscsc_phi     =            %8.2f\n", energyTerms[49]);

  printf("interD_vdwatt         =            %8.2f\n", energyTerms[51]);
  printf("interD_vdwrep         =            %8.2f\n", energyTerms[52]);
  printf("interD_electr         =            %8.2f\n", energyTerms[53]);
  printf("interD_deslvP         =            %8.2f\n", energyTerms[54]);
  printf("interD_deslvH         =            %8.2f\n", energyTerms[55]);
  printf("interD_ssbond         =            %8.2f\n", energyTerms[56]);
  printf("interD_hbbbbb_dis     =            %8.2f\n", energyTerms[61]);
  printf("interD_hbbbbb_the     =            %8.2f\n", energyTerms[62]);
  printf("interD_hbbbbb_phi     =            %8.2f\n", energyTerms[63]);
  printf("interD_hbscbb_dis     =            %8.2f\n", energyTerms[64]);
  printf("interD_hbscbb_the     =            %8.2f\n", energyTerms[65]);
  printf("interD_hbscbb_phi     =            %8.2f\n", energyTerms[66]);
  printf("interD_hbscsc_dis     =            %8.2f\n", energyTerms[67]);
  printf("interD_hbscsc_the     =            %8.2f\n", energyTerms[68]);
  printf("interD_hbscsc_phi     =            %8.2f\n", energyTerms[69]);
  printf("----------------------------------------------------\n");
  printf("Total                 =            %8.2f\n\n", energyTerms[0]);
  return Success;
}


int NOVA_ComputeStabilityWithBBdepRotLib(Structure *pStructure,AAppTable* pAAppTable,RamaTable* pRama,BBdepRotamerLib* pRotLib,double energyTerms[MAX_EVOEF_ENERGY_TERM_NUM]){
  //EnergyTermInitialize(energyTerms);
  int aas[20]={0}; //ACDEFGHIKLMNPQRSTVWY, only for regular amino acid
  //StructureGetAminoAcidComposition(pStructure, aas);
  // if the structure is composed of several chains, the residue position could be different in the whole structure from that in the separate chain
  //StructureComputeResiduePosition(pStructure);
  for(int i = 0; i < StructureGetChainCount(pStructure); i++){
    Chain *pChainI = StructureGetChain(pStructure,i);
    for(int ir = 0; ir < ChainGetResidueCount(pChainI); ir++){
      Residue *pResIR = ChainGetResidue(pChainI,ir);
      double refer=0.0;
      EVOEF_AminoAcidReferenceEnergy(pResIR->name, energyTerms);
      EVOEF_EnergyResidueIntraEnergy(pResIR,energyTerms);
      //amino acid propensity and ramachandran energy
      AminoAcidPropensityAndRamachandranEnergy(pResIR,pAAppTable,pRama,energyTerms);
      //dunbrack energy
      AminoAcidDunbrackEnergy(pResIR,pRotLib,energyTerms);
      for(int is = ir+1; is < ChainGetResidueCount(pChainI); is++){
        Residue *pResIS = ChainGetResidue(pChainI,is);
        if(is==ir+1) EVOEF_EnergyResidueAndNextResidue(pResIR,pResIS,energyTerms);
        else EVOEF_EnergyResidueAndOtherResidueSameChain(pResIR,pResIS,energyTerms);
      }
      for(int k = i+1; k < StructureGetChainCount(pStructure); k++){
        Chain *pChainK = StructureGetChain(pStructure,k);
        for(int ks = 0; ks < ChainGetResidueCount(pChainK); ks++){
          Residue *pResKS = ChainGetResidue(pChainK,ks);
          EVOEF_EnergyResidueAndOtherResidueDifferentChain(pResIR,pResKS,energyTerms);
        }
      }
    }
  }

  EnergyTermWeighting(energyTerms);
  printf("\nStructure energy details:\n");
  printf("reference_ALA         =            %8.2f\n", energyTerms[ 1]);
  printf("reference_CYS         =            %8.2f\n", energyTerms[ 2]);
  printf("reference_ASP         =            %8.2f\n", energyTerms[ 3]);
  printf("reference_GLU         =            %8.2f\n", energyTerms[ 4]);
  printf("reference_PHE         =            %8.2f\n", energyTerms[ 5]);
  printf("reference_GLY         =            %8.2f\n", energyTerms[ 6]);
  printf("reference_HIS         =            %8.2f\n", energyTerms[ 7]);
  printf("reference_ILE         =            %8.2f\n", energyTerms[ 8]);
  printf("reference_LYS         =            %8.2f\n", energyTerms[ 9]);
  printf("reference_LEU         =            %8.2f\n", energyTerms[10]);
  printf("reference_MET         =            %8.2f\n", energyTerms[11]);
  printf("reference_ASN         =            %8.2f\n", energyTerms[12]);
  printf("reference_PRO         =            %8.2f\n", energyTerms[13]);
  printf("reference_GLN         =            %8.2f\n", energyTerms[14]);
  printf("reference_ARG         =            %8.2f\n", energyTerms[15]);
  printf("reference_SER         =            %8.2f\n", energyTerms[16]);
  printf("reference_THR         =            %8.2f\n", energyTerms[17]);
  printf("reference_VAL         =            %8.2f\n", energyTerms[18]);
  printf("reference_TRP         =            %8.2f\n", energyTerms[19]);
  printf("reference_TYR         =            %8.2f\n", energyTerms[20]);

  printf("intraR_vdwatt         =            %8.2f\n", energyTerms[21]);
  printf("intraR_vdwrep         =            %8.2f\n", energyTerms[22]);
  printf("intraR_electr         =            %8.2f\n", energyTerms[23]);
  printf("intraR_deslvP         =            %8.2f\n", energyTerms[24]);
  printf("intraR_deslvH         =            %8.2f\n", energyTerms[25]);
  printf("intraR_hbscbb_dis     =            %8.2f\n", energyTerms[26]);
  printf("intraR_hbscbb_the     =            %8.2f\n", energyTerms[27]);
  printf("intraR_hbscbb_phi     =            %8.2f\n", energyTerms[28]);
  printf("aapropensity          =            %8.2f\n", energyTerms[91]);
  printf("ramachandran          =            %8.2f\n", energyTerms[92]);
  printf("dunbrack              =            %8.2f\n", energyTerms[93]);

  printf("interS_vdwatt         =            %8.2f\n", energyTerms[31]);
  printf("interS_vdwrep         =            %8.2f\n", energyTerms[32]);
  printf("interS_electr         =            %8.2f\n", energyTerms[33]);
  printf("interS_deslvP         =            %8.2f\n", energyTerms[34]);
  printf("interS_deslvH         =            %8.2f\n", energyTerms[35]);
  printf("interS_ssbond         =            %8.2f\n", energyTerms[36]);
  printf("interS_hbbbbb_dis     =            %8.2f\n", energyTerms[41]);
  printf("interS_hbbbbb_the     =            %8.2f\n", energyTerms[42]);
  printf("interS_hbbbbb_phi     =            %8.2f\n", energyTerms[43]);
  printf("interS_hbscbb_dis     =            %8.2f\n", energyTerms[44]);
  printf("interS_hbscbb_the     =            %8.2f\n", energyTerms[45]);
  printf("interS_hbscbb_phi     =            %8.2f\n", energyTerms[46]);
  printf("interS_hbscsc_dis     =            %8.2f\n", energyTerms[47]);
  printf("interS_hbscsc_the     =            %8.2f\n", energyTerms[48]);
  printf("interS_hbscsc_phi     =            %8.2f\n", energyTerms[49]);

  printf("interD_vdwatt         =            %8.2f\n", energyTerms[51]);
  printf("interD_vdwrep         =            %8.2f\n", energyTerms[52]);
  printf("interD_electr         =            %8.2f\n", energyTerms[53]);
  printf("interD_deslvP         =            %8.2f\n", energyTerms[54]);
  printf("interD_deslvH         =            %8.2f\n", energyTerms[55]);
  printf("interD_ssbond         =            %8.2f\n", energyTerms[56]);
  printf("interD_hbbbbb_dis     =            %8.2f\n", energyTerms[61]);
  printf("interD_hbbbbb_the     =            %8.2f\n", energyTerms[62]);
  printf("interD_hbbbbb_phi     =            %8.2f\n", energyTerms[63]);
  printf("interD_hbscbb_dis     =            %8.2f\n", energyTerms[64]);
  printf("interD_hbscbb_the     =            %8.2f\n", energyTerms[65]);
  printf("interD_hbscbb_phi     =            %8.2f\n", energyTerms[66]);
  printf("interD_hbscsc_dis     =            %8.2f\n", energyTerms[67]);
  printf("interD_hbscsc_the     =            %8.2f\n", energyTerms[68]);
  printf("interD_hbscsc_phi     =            %8.2f\n", energyTerms[69]);
  printf("----------------------------------------------------\n");
  printf("Total                 =            %8.2f\n\n", energyTerms[0]);
  return Success;
}

int NOVA_ComputeBinding(Structure *pStructure){
  if(StructureGetChainCount(pStructure)>2){
    printf("Your structure has more than two protein chains, and you should specify how to split chains "
      "before computing the binding energy\n");
    printf("Otherwise, EvoEF just output the interactions between any chain pair (DEFAULT)\n");
  }
  else if(StructureGetChainCount(pStructure)<=1){
    printf("Your structure has less than or equal to one chain, binding energy cannot be calculated\n");
    return Warning;
  }

  for(int i=0; i<StructureGetChainCount(pStructure);i++){
    Chain* pChainI=StructureGetChain(pStructure,i);
    for(int k=i+1; k<StructureGetChainCount(pStructure);k++){
      Chain* pChainK=StructureGetChain(pStructure,k);
      double energyTerms[MAX_EVOEF_ENERGY_TERM_NUM]={0};
      for(int j=0;j<ChainGetResidueCount(pChainI);j++){
        Residue* pResiIJ=ChainGetResidue(pChainI,j);
        for(int s=0;s<ChainGetResidueCount(pChainK);s++){
          Residue* pResiKS=ChainGetResidue(pChainK,s);
          EVOEF_EnergyResidueAndOtherResidueDifferentChain(pResiIJ,pResiKS,energyTerms);
        }
      }
      EnergyTermWeighting(energyTerms);
      // energy terms are weighted during the calculation, don't weight them for the difference
      printf("Binding energy details between chain(s) %s and chain(s) %s:\n",
        ChainGetName(pChainI),ChainGetName(pChainK),ChainGetName(pChainI),ChainGetName(pChainK));
      printf("reference_ALA         =            %8.2f\n", energyTerms[ 1]);
      printf("reference_CYS         =            %8.2f\n", energyTerms[ 2]);
      printf("reference_ASP         =            %8.2f\n", energyTerms[ 3]);
      printf("reference_GLU         =            %8.2f\n", energyTerms[ 4]);
      printf("reference_PHE         =            %8.2f\n", energyTerms[ 5]);
      printf("reference_GLY         =            %8.2f\n", energyTerms[ 6]);
      printf("reference_HIS         =            %8.2f\n", energyTerms[ 7]);
      printf("reference_ILE         =            %8.2f\n", energyTerms[ 8]);
      printf("reference_LYS         =            %8.2f\n", energyTerms[ 9]);
      printf("reference_LEU         =            %8.2f\n", energyTerms[10]);
      printf("reference_MET         =            %8.2f\n", energyTerms[11]);
      printf("reference_ASN         =            %8.2f\n", energyTerms[12]);
      printf("reference_PRO         =            %8.2f\n", energyTerms[13]);
      printf("reference_GLN         =            %8.2f\n", energyTerms[14]);
      printf("reference_ARG         =            %8.2f\n", energyTerms[15]);
      printf("reference_SER         =            %8.2f\n", energyTerms[16]);
      printf("reference_THR         =            %8.2f\n", energyTerms[17]);
      printf("reference_VAL         =            %8.2f\n", energyTerms[18]);
      printf("reference_TRP         =            %8.2f\n", energyTerms[19]);
      printf("reference_TYR         =            %8.2f\n", energyTerms[20]);
      printf("intraR_vdwatt         =            %8.2f\n", energyTerms[21]);
      printf("intraR_vdwrep         =            %8.2f\n", energyTerms[22]);
      printf("intraR_electr         =            %8.2f\n", energyTerms[23]);
      printf("intraR_deslvP         =            %8.2f\n", energyTerms[24]);
      printf("intraR_deslvH         =            %8.2f\n", energyTerms[25]);
      printf("intraR_hbscbb_dis     =            %8.2f\n", energyTerms[26]);
      printf("intraR_hbscbb_the     =            %8.2f\n", energyTerms[27]);
      printf("intraR_hbscbb_phi     =            %8.2f\n", energyTerms[28]);
      printf("interS_vdwatt         =            %8.2f\n", energyTerms[31]);
      printf("interS_vdwrep         =            %8.2f\n", energyTerms[32]);
      printf("interS_electr         =            %8.2f\n", energyTerms[33]);
      printf("interS_deslvP         =            %8.2f\n", energyTerms[34]);
      printf("interS_deslvH         =            %8.2f\n", energyTerms[35]);
      printf("interS_ssbond         =            %8.2f\n", energyTerms[36]);
      printf("interS_hbbbbb_dis     =            %8.2f\n", energyTerms[41]);
      printf("interS_hbbbbb_the     =            %8.2f\n", energyTerms[42]);
      printf("interS_hbbbbb_phi     =            %8.2f\n", energyTerms[43]);
      printf("interS_hbscbb_dis     =            %8.2f\n", energyTerms[44]);
      printf("interS_hbscbb_the     =            %8.2f\n", energyTerms[45]);
      printf("interS_hbscbb_phi     =            %8.2f\n", energyTerms[46]);
      printf("interS_hbscsc_dis     =            %8.2f\n", energyTerms[47]);
      printf("interS_hbscsc_the     =            %8.2f\n", energyTerms[48]);
      printf("interS_hbscsc_phi     =            %8.2f\n", energyTerms[49]);
      printf("interD_vdwatt         =            %8.2f\n", energyTerms[51]);
      printf("interD_vdwrep         =            %8.2f\n", energyTerms[52]);
      printf("interD_electr         =            %8.2f\n", energyTerms[53]);
      printf("interD_deslvP         =            %8.2f\n", energyTerms[54]);
      printf("interD_deslvH         =            %8.2f\n", energyTerms[55]);
      printf("interD_ssbond         =            %8.2f\n", energyTerms[56]);
      printf("interD_hbbbbb_dis     =            %8.2f\n", energyTerms[61]);
      printf("interD_hbbbbb_the     =            %8.2f\n", energyTerms[62]);
      printf("interD_hbbbbb_phi     =            %8.2f\n", energyTerms[63]);
      printf("interD_hbscbb_dis     =            %8.2f\n", energyTerms[64]);
      printf("interD_hbscbb_the     =            %8.2f\n", energyTerms[65]);
      printf("interD_hbscbb_phi     =            %8.2f\n", energyTerms[66]);
      printf("interD_hbscsc_dis     =            %8.2f\n", energyTerms[67]);
      printf("interD_hbscsc_the     =            %8.2f\n", energyTerms[68]);
      printf("interD_hbscsc_phi     =            %8.2f\n", energyTerms[69]);
      printf("----------------------------------------------------\n");
      printf("Total                 =            %8.2f\n", energyTerms[0]);
    }
  }

  return Success;
}


int NOVA_ComputeBindingWithSplittingNew(Structure *pStructure,char split1[], char split2[]){
  double energyTerms[MAX_EVOEF_ENERGY_TERM_NUM]={0};
  for(int i=0;i<StructureGetChainCount(pStructure);i++){
    Chain* pChainI=StructureGetChain(pStructure,i);
    for(int k=i+1;k<StructureGetChainCount(pStructure);k++){
      Chain* pChainK=StructureGetChain(pStructure,k);
      if((strstr(split1,ChainGetName(pChainI))!=NULL && strstr(split2,ChainGetName(pChainK))!=NULL)||
        ((strstr(split2,ChainGetName(pChainI))!=NULL && strstr(split1,ChainGetName(pChainK))!=NULL))){
          for(int j=0;j<ChainGetResidueCount(pChainI);j++){
            Residue* pResiIJ=ChainGetResidue(pChainI,j);
            for(int s=0;s<ChainGetResidueCount(pChainK);s++){
              Residue* pResiKS=ChainGetResidue(pChainK,s);
              EVOEF_EnergyResidueAndOtherResidueDifferentChain(pResiIJ,pResiKS,energyTerms);
            }
          }
      }
    }
  }
  EnergyTermWeighting(energyTerms);
  // energy terms are weighted during the calculation, don't weight them for the difference
  printf("Binding energy details between chain(s) %s and chain(s) %s (DG_bind = DG(stability,complex) - DG(stability,%s) - DG(stability,%s):\n",split1,split2,split1,split2);
  printf("reference_ALA         =            %8.2f\n", energyTerms[ 1]);
  printf("reference_CYS         =            %8.2f\n", energyTerms[ 2]);
  printf("reference_ASP         =            %8.2f\n", energyTerms[ 3]);
  printf("reference_GLU         =            %8.2f\n", energyTerms[ 4]);
  printf("reference_PHE         =            %8.2f\n", energyTerms[ 5]);
  printf("reference_GLY         =            %8.2f\n", energyTerms[ 6]);
  printf("reference_HIS         =            %8.2f\n", energyTerms[ 7]);
  printf("reference_ILE         =            %8.2f\n", energyTerms[ 8]);
  printf("reference_LYS         =            %8.2f\n", energyTerms[ 9]);
  printf("reference_LEU         =            %8.2f\n", energyTerms[10]);
  printf("reference_MET         =            %8.2f\n", energyTerms[11]);
  printf("reference_ASN         =            %8.2f\n", energyTerms[12]);
  printf("reference_PRO         =            %8.2f\n", energyTerms[13]);
  printf("reference_GLN         =            %8.2f\n", energyTerms[14]);
  printf("reference_ARG         =            %8.2f\n", energyTerms[15]);
  printf("reference_SER         =            %8.2f\n", energyTerms[16]);
  printf("reference_THR         =            %8.2f\n", energyTerms[17]);
  printf("reference_VAL         =            %8.2f\n", energyTerms[18]);
  printf("reference_TRP         =            %8.2f\n", energyTerms[19]);
  printf("reference_TYR         =            %8.2f\n", energyTerms[20]);
  printf("intraR_vdwatt         =            %8.2f\n", energyTerms[21]);
  printf("intraR_vdwrep         =            %8.2f\n", energyTerms[22]);
  printf("intraR_electr         =            %8.2f\n", energyTerms[23]);
  printf("intraR_deslvP         =            %8.2f\n", energyTerms[24]);
  printf("intraR_deslvH         =            %8.2f\n", energyTerms[25]);
  printf("intraR_hbscbb_dis     =            %8.2f\n", energyTerms[26]);
  printf("intraR_hbscbb_the     =            %8.2f\n", energyTerms[27]);
  printf("intraR_hbscbb_phi     =            %8.2f\n", energyTerms[28]);
  printf("interS_vdwatt         =            %8.2f\n", energyTerms[31]);
  printf("interS_vdwrep         =            %8.2f\n", energyTerms[32]);
  printf("interS_electr         =            %8.2f\n", energyTerms[33]);
  printf("interS_deslvP         =            %8.2f\n", energyTerms[34]);
  printf("interS_deslvH         =            %8.2f\n", energyTerms[35]);
  printf("interS_ssbond         =            %8.2f\n", energyTerms[36]);
  printf("interS_hbbbbb_dis     =            %8.2f\n", energyTerms[41]);
  printf("interS_hbbbbb_the     =            %8.2f\n", energyTerms[42]);
  printf("interS_hbbbbb_phi     =            %8.2f\n", energyTerms[43]);
  printf("interS_hbscbb_dis     =            %8.2f\n", energyTerms[44]);
  printf("interS_hbscbb_the     =            %8.2f\n", energyTerms[45]);
  printf("interS_hbscbb_phi     =            %8.2f\n", energyTerms[46]);
  printf("interS_hbscsc_dis     =            %8.2f\n", energyTerms[47]);
  printf("interS_hbscsc_the     =            %8.2f\n", energyTerms[48]);
  printf("interS_hbscsc_phi     =            %8.2f\n", energyTerms[49]);
  printf("interD_vdwatt         =            %8.2f\n", energyTerms[51]);
  printf("interD_vdwrep         =            %8.2f\n", energyTerms[52]);
  printf("interD_electr         =            %8.2f\n", energyTerms[53]);
  printf("interD_deslvP         =            %8.2f\n", energyTerms[54]);
  printf("interD_deslvH         =            %8.2f\n", energyTerms[55]);
  printf("interD_ssbond         =            %8.2f\n", energyTerms[56]);
  printf("interD_hbbbbb_dis     =            %8.2f\n", energyTerms[61]);
  printf("interD_hbbbbb_the     =            %8.2f\n", energyTerms[62]);
  printf("interD_hbbbbb_phi     =            %8.2f\n", energyTerms[63]);
  printf("interD_hbscbb_dis     =            %8.2f\n", energyTerms[64]);
  printf("interD_hbscbb_the     =            %8.2f\n", energyTerms[65]);
  printf("interD_hbscbb_phi     =            %8.2f\n", energyTerms[66]);
  printf("interD_hbscsc_dis     =            %8.2f\n", energyTerms[67]);
  printf("interD_hbscsc_the     =            %8.2f\n", energyTerms[68]);
  printf("interD_hbscsc_phi     =            %8.2f\n", energyTerms[69]);
  printf("----------------------------------------------------\n");
  printf("Total                 =            %8.2f\n", energyTerms[0]);
  return Success;
}


//this function is used to build the structure model of mutations
int NOVA_BuildMutant(Structure* pStructure, char* mutantfile, BBindRotamerLib* rotlib, AtomParamsSet* atomParams,ResiTopoSet* resiTopos, char* pdbid){
  FileReader fr;
  FileReaderCreate(&fr, mutantfile);
  int mutantcount = FileReaderGetLineCount(&fr);
  if(mutantcount<=0){
    printf("There is no mutant found in the mutant file\n");
    return DataNotExistError;
  }

  StringArray* mutants = (StringArray*)malloc(sizeof(StringArray)*mutantcount);
  char line[MAX_LENGTH_ONE_LINE_IN_FILE+1];
  int mutantIndex=0;
  while(!FAILED(FileReaderGetNextLine(&fr, line))){
    StringArrayCreate(&mutants[mutantIndex]);
    StringArraySplitString(&mutants[mutantIndex], line, ',');
    char lastMutant[MAX_LENGTH_ONE_LINE_IN_FILE+1];
    int lastmutindex = StringArrayGetCount(&mutants[mutantIndex])-1;
    strcpy(lastMutant, StringArrayGet(&mutants[mutantIndex], lastmutindex));
    //deal with the last char of the last single mutant
    if((!isdigit(lastMutant[strlen(lastMutant)-1])) && !isalpha(lastMutant[strlen(lastMutant)-1])){
      lastMutant[strlen(lastMutant)-1] = '\0';
    }
    StringArraySet(&mutants[mutantIndex], lastmutindex, lastMutant);
    mutantIndex++;
  }
  FileReaderDestroy(&fr);

  for(int mutantIndex = 0; mutantIndex < mutantcount; mutantIndex++){
    //for each mutant, build the rotamer-tree
    IntArray mutantArray,rotamersArray;
    IntArrayCreate(&mutantArray,0);
    IntArrayCreate(&rotamersArray,0);
    for(int cycle=0; cycle<StringArrayGetCount(&mutants[mutantIndex]); cycle++){
      char mutstr[10];
      char aa1, chn, aa2;
      int posInChain;
      strcpy(mutstr, StringArrayGet(&mutants[mutantIndex], cycle));
      sscanf(mutstr, "%c%c%d%c", &aa1, &chn, &posInChain, &aa2);
      int chainIndex = -1, residueIndex = -1;
      char chainname[MAX_LENGTH_CHAIN_NAME]; chainname[0] = chn; chainname[1] = '\0';
      StructureFindChainIndex(pStructure, chainname, &chainIndex);
      if(chainIndex==-1){
        printf("in file %s function %s() line %d, cannot find mutation %s\n", __FILE__, __FUNCTION__, __LINE__, mutstr);
        exit(ValueError);
      }
      ChainFindResidueByPosInChain(StructureGetChain(pStructure, chainIndex), posInChain, &residueIndex);
      if(residueIndex==-1){
        printf("in file %s function %s() line %d, cannot find mutation %s\n", __FILE__, __FUNCTION__, __LINE__, mutstr);
        exit(ValueError);
      }
      char mutaatype[MAX_LENGTH_RESIDUE_NAME];
      OneLetterAAToThreeLetterAA(aa2, mutaatype);
      StringArray designType, patchType;
      StringArrayCreate(&designType);
      StringArrayCreate(&patchType);
      // for histidine, the default mutaatype is HSD, we need to add HSE
      StringArrayAppend(&designType, mutaatype); StringArrayAppend(&patchType, "");
      if(aa2=='H'){StringArrayAppend(&designType, "HSE"); StringArrayAppend(&patchType, "");}
      ProteinSiteBuildMutatedRotamers(pStructure,chainIndex,residueIndex,rotlib,atomParams,resiTopos,&designType,&patchType);
      IntArrayAppend(&mutantArray, chainIndex);
      IntArrayAppend(&mutantArray, residueIndex);
      IntArrayAppend(&rotamersArray,chainIndex);
      IntArrayAppend(&rotamersArray,residueIndex);
      StringArrayDestroy(&designType);
      StringArrayDestroy(&patchType);
    }

    // for each mutant, find the surrounding residues and build the wild-type rotamer-tree
    for(int ii=0; ii<IntArrayGetLength(&mutantArray); ii+=2){
      int chainIndex = IntArrayGet(&mutantArray,ii);
      int resiIndex = IntArrayGet(&mutantArray,ii+1);
      Residue *pResi1 = ChainGetResidue(StructureGetChain(pStructure, chainIndex), resiIndex);
      for(int j = 0; j < StructureGetChainCount(pStructure); ++j){
        Chain* pChain = StructureGetChain(pStructure,j);
        for(int k=0; k<ChainGetResidueCount(pChain); k++){
          Residue* pResi2 = ChainGetResidue(pChain,k);
          if(AtomArrayCalcMinDistance(&pResi1->atoms,&pResi2->atoms)<ENERGY_DISTANCE_CUTOFF){
            if(pResi2->designSiteType==Type_ResidueDesignType_Fixed){
              ProteinSiteBuildWildtypeRotamers(pStructure,j,k,rotlib,atomParams,resiTopos);
              ProteinSiteAddCrystalRotamer(pStructure,j,k,resiTopos);
              IntArrayAppend(&rotamersArray,j);
              IntArrayAppend(&rotamersArray,k);
            }
          }
        }
      }
    }

    // optimization rotamers sequentially
    printf("EvoEF Building mutation model %d, the following sites will be optimized:\n",mutantIndex+1);
    IntArrayShow(&rotamersArray);
    printf("\n");
    for(int cycle=0; cycle<3; cycle++){
      printf("optimization cycle %d ... \n",cycle+1);
      for(int ii=0; ii<IntArrayGetLength(&rotamersArray); ii+=2){
        int chainIndex = IntArrayGet(&rotamersArray, ii);
        int resiIndex = IntArrayGet(&rotamersArray, ii+1);
        //ProteinSiteOptimizeRotamer(pStructure, chainIndex, resiIndex);
        ProteinSiteOptimizeRotamerLocally(pStructure,chainIndex,resiIndex,1.0);
      }
    }
    IntArrayDestroy(&mutantArray);
    IntArrayDestroy(&rotamersArray);
    //remember to delete rotamers for previous mutant
    StructureRemoveAllDesignSites(pStructure);

    char modelfile[MAX_LENGTH_ONE_LINE_IN_FILE+1];
    if(pdbid!=NULL)
      sprintf(modelfile,"%s_Model_%d.pdb",pdbid,mutantIndex+1);
    else
      sprintf(modelfile,"EvoEF_Model_%d.pdb",mutantIndex+1);
    FILE* pf=fopen(modelfile,"w");
    fprintf(pf,"REMARK EvoEF generated pdb file\n");
    fprintf(pf,"REMARK Output generated by EvoEF <BuildMutant>\n");
    StructureShowInPDBFormat(pStructure,TRUE,pf);
    fclose(pf);
  }

  return Success;
}


int NOVA_BuildMutantWithBBdepRotLib(Structure* pStructure, char* mutantfile, BBdepRotamerLib* pBBdepRotLib, AtomParamsSet* atomParams,ResiTopoSet* resiTopos, char* pdbid){
  FileReader fr;
  FileReaderCreate(&fr, mutantfile);
  int mutantcount = FileReaderGetLineCount(&fr);
  if(mutantcount<=0){
    printf("There is no mutant found in the mutant file\n");
    return DataNotExistError;
  }

  StringArray* mutants = (StringArray*)malloc(sizeof(StringArray)*mutantcount);
  char line[MAX_LENGTH_ONE_LINE_IN_FILE+1];
  int mutantIndex=0;
  while(!FAILED(FileReaderGetNextLine(&fr, line))){
    StringArrayCreate(&mutants[mutantIndex]);
    StringArraySplitString(&mutants[mutantIndex], line, ',');
    char lastMutant[MAX_LENGTH_ONE_LINE_IN_FILE+1];
    int lastmutindex = StringArrayGetCount(&mutants[mutantIndex])-1;
    strcpy(lastMutant, StringArrayGet(&mutants[mutantIndex], lastmutindex));
    //deal with the last char of the last single mutant
    if((!isdigit(lastMutant[strlen(lastMutant)-1])) && !isalpha(lastMutant[strlen(lastMutant)-1])){
      lastMutant[strlen(lastMutant)-1] = '\0';
    }
    StringArraySet(&mutants[mutantIndex], lastmutindex, lastMutant);
    mutantIndex++;
  }
  FileReaderDestroy(&fr);

  for(int mutantIndex = 0; mutantIndex < mutantcount; mutantIndex++){
    //for each mutant, build the rotamer-tree
    IntArray mutantArray,rotamersArray;
    IntArrayCreate(&mutantArray,0);
    IntArrayCreate(&rotamersArray,0);
    for(int cycle=0; cycle<StringArrayGetCount(&mutants[mutantIndex]); cycle++){
      char mutstr[10];
      char aa1, chn, aa2;
      int posInChain;
      strcpy(mutstr, StringArrayGet(&mutants[mutantIndex], cycle));
      sscanf(mutstr, "%c%c%d%c", &aa1, &chn, &posInChain, &aa2);
      int chainIndex = -1, residueIndex = -1;
      char chainname[MAX_LENGTH_CHAIN_NAME]; chainname[0] = chn; chainname[1] = '\0';
      StructureFindChainIndex(pStructure, chainname, &chainIndex);
      if(chainIndex==-1){
        printf("in file %s function %s() line %d, cannot find mutation %s\n", __FILE__, __FUNCTION__, __LINE__, mutstr);
        exit(ValueError);
      }
      ChainFindResidueByPosInChain(StructureGetChain(pStructure, chainIndex), posInChain, &residueIndex);
      if(residueIndex==-1){
        printf("in file %s function %s() line %d, cannot find mutation %s\n", __FILE__, __FUNCTION__, __LINE__, mutstr);
        exit(ValueError);
      }
      char mutaatype[MAX_LENGTH_RESIDUE_NAME];
      OneLetterAAToThreeLetterAA(aa2, mutaatype);
      StringArray designType, patchType;
      StringArrayCreate(&designType);
      StringArrayCreate(&patchType);
      // for histidine, the default mutaatype is HSD, we need to add HSE
      StringArrayAppend(&designType, mutaatype); StringArrayAppend(&patchType, "");
      if(aa2=='H'){StringArrayAppend(&designType, "HSE"); StringArrayAppend(&patchType, "");}
      ProteinSiteBuildMutatedRotamersByBBdepRotLib(pStructure,chainIndex,residueIndex,pBBdepRotLib,atomParams,resiTopos,&designType,&patchType);
      IntArrayAppend(&mutantArray, chainIndex);
      IntArrayAppend(&mutantArray, residueIndex);
      IntArrayAppend(&rotamersArray,chainIndex);
      IntArrayAppend(&rotamersArray,residueIndex);
      StringArrayDestroy(&designType);
      StringArrayDestroy(&patchType);
    }

    // for each mutant, find the surrounding residues and build the wild-type rotamer-tree
    for(int ii=0; ii<IntArrayGetLength(&mutantArray); ii+=2){
      int chainIndex = IntArrayGet(&mutantArray,ii);
      int resiIndex = IntArrayGet(&mutantArray,ii+1);
      Residue *pResi1 = ChainGetResidue(StructureGetChain(pStructure, chainIndex), resiIndex);
      for(int j = 0; j < StructureGetChainCount(pStructure); ++j){
        Chain* pChain = StructureGetChain(pStructure,j);
        for(int k=0; k<ChainGetResidueCount(pChain); k++){
          Residue* pResi2 = ChainGetResidue(pChain,k);
          if(AtomArrayCalcMinDistance(&pResi1->atoms,&pResi2->atoms)<ENERGY_DISTANCE_CUTOFF){
            if(pResi2->designSiteType==Type_ResidueDesignType_Fixed){
              ProteinSiteBuildWildtypeRotamersByBBdepRotLib(pStructure,j,k,pBBdepRotLib,atomParams,resiTopos);
              ProteinSiteAddCrystalRotamerByBBdepRotLib(pStructure,j,k,resiTopos,pBBdepRotLib);
              IntArrayAppend(&rotamersArray,j);
              IntArrayAppend(&rotamersArray,k);
            }
          }
        }
      }
    }

    // optimization rotamers sequentially
    printf("EvoEF Building mutation model %d, the following sites will be optimized:\n",mutantIndex+1);
    IntArrayShow(&rotamersArray);
    printf("\n");
    for(int cycle=0; cycle<3; cycle++){
      printf("optimization cycle %d ... \n",cycle+1);
      for(int ii=0; ii<IntArrayGetLength(&rotamersArray); ii+=2){
        int chainIndex = IntArrayGet(&rotamersArray, ii);
        int resiIndex = IntArrayGet(&rotamersArray, ii+1);
        ProteinSiteOptimizeRotamerWithBBdepRotLib(pStructure, chainIndex, resiIndex,pBBdepRotLib);
        //ProteinSiteOptimizeRotamerLocallyWithBBdepRotLib(pStructure,chainIndex,resiIndex,1.0,pBBdepRotLib);
      }
    }
    IntArrayDestroy(&mutantArray);
    IntArrayDestroy(&rotamersArray);
    //remember to delete rotamers for previous mutant
    StructureRemoveAllDesignSites(pStructure);

    char modelfile[MAX_LENGTH_ONE_LINE_IN_FILE+1];
    if(pdbid!=NULL)
      sprintf(modelfile,"%s_Model_%d.pdb",pdbid,mutantIndex+1);
    else
      sprintf(modelfile,"EvoEF_Model_%d.pdb",mutantIndex+1);
    FILE* pf=fopen(modelfile,"w");
    fprintf(pf,"REMARK EvoEF generated pdb file\n");
    fprintf(pf,"REMARK Output generated by EvoEF <BuildMutant>\n");
    StructureShowInPDBFormat(pStructure,TRUE,pf);
    fclose(pf);
  }

  return Success;
}


int NOVA_RepairStructure(Structure* pStructure, BBindRotamerLib* pBBindRotLib, AtomParamsSet* atomParams,ResiTopoSet* resiTopos,char* pdbid){
  for(int cycle=0; cycle<1; cycle++){
    printf("EvoEF Repairing PDB: optimization cycle %d ... \n",cycle+1);
    for(int i=0; i<StructureGetChainCount(pStructure); ++i){
      Chain* pChain = StructureGetChain(pStructure, i);
      for(int j=0; j<ChainGetResidueCount(pChain); j++){
        Residue* pResi = ChainGetResidue(pChain, j);
        if(strcmp(ResidueGetName(pResi),"ALA")==0 || strcmp(ResidueGetName(pResi),"GLY")==0) continue;
        //skip CYS which may form disulfide bonds
        if(strcmp(ResidueGetName(pResi),"CYS")==0) continue;
        if(strcmp(ResidueGetName(pResi),"ASN")==0||strcmp(ResidueGetName(pResi),"GLN")==0||strcmp(ResidueGetName(pResi),"HSD")==0||strcmp(ResidueGetName(pResi),"HSE")==0){
          printf("We will flip residue %s%d%c to optimize hbond\n", ResidueGetChainName(pResi),ResidueGetPosInChain(pResi),ThreeLetterAAToOneLetterAA(ResidueGetName(pResi)));
          ProteinSiteAddCrystalRotamer(pStructure,i,j,resiTopos);
          ProteinSiteBuildFlippedCrystalRotamer(pStructure,i,j,resiTopos);
          ProteinSiteOptimizeRotamerHBondEnergy(pStructure,i,j);
        }
        else if(strcmp(ResidueGetName(pResi),"SER")==0 || strcmp(ResidueGetName(pResi),"THR")==0 || strcmp(ResidueGetName(pResi),"TYR")==0){
          printf("We will rotate hydroxyl group of residue %s%d%c to optimize hbond\n", ResidueGetChainName(pResi),ResidueGetPosInChain(pResi),ThreeLetterAAToOneLetterAA(ResidueGetName(pResi)));
          ProteinSiteAddCrystalRotamer(pStructure,i,j,resiTopos);
          ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
          ProteinSiteOptimizeRotamerHBondEnergy(pStructure,i,j);
        }
        if(TRUE){
          printf("We optimize side chain of residue %s%d%c\n", ResidueGetChainName(pResi),ResidueGetPosInChain(pResi),ThreeLetterAAToOneLetterAA(ResidueGetName(pResi)));
          ProteinSiteBuildWildtypeRotamers(pStructure,i,j,pBBindRotLib,atomParams,resiTopos);
          ProteinSiteAddCrystalRotamer(pStructure,i,j,resiTopos);
          ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
          ProteinSiteOptimizeRotamer(pStructure,i,j);
          //ProteinSiteOptimizeRotamerLocally(pStructure,i,j, 1.0);
        }
        ProteinSiteRemoveDesignSite(pStructure,i,j);
      }
    }
  }

  //output the repaired structure
  char modelfile[MAX_LENGTH_ONE_LINE_IN_FILE+1];
  if(pdbid!=NULL){sprintf(modelfile,"%s_Repair.pdb",pdbid);}
  else{strcpy(modelfile,"EvoEF_Repair.pdb");}
  FILE* pf=fopen(modelfile,"w");
  fprintf(pf,"REMARK EvoEF generated pdb file\n");
  fprintf(pf,"REMARK Output generated by EvoEF <RepairStructure>\n");
  StructureShowInPDBFormat(pStructure,TRUE,pf);
  fclose(pf);

  return Success;
}


int NOVA_RepairStructureWithBBdepRotLib(Structure* pStructure, BBdepRotamerLib* pBBdepRotLib, AtomParamsSet* atomParams,ResiTopoSet* resiTopos,char* pdbid){
  for(int cycle=0; cycle<1; cycle++){
    printf("EvoEF Repairing PDB: optimization cycle %d ... \n",cycle+1);
    for(int i=0; i<StructureGetChainCount(pStructure); ++i){
      Chain* pChain = StructureGetChain(pStructure, i);
      for(int j=0; j<ChainGetResidueCount(pChain); j++){
        Residue* pResi = ChainGetResidue(pChain, j);
        if(strcmp(ResidueGetName(pResi),"ALA")==0 || strcmp(ResidueGetName(pResi),"GLY")==0) continue;
        //skip CYS which may form disulfide bonds
        if(strcmp(ResidueGetName(pResi),"CYS")==0) continue;
        if(strcmp(ResidueGetName(pResi),"ASN")==0||strcmp(ResidueGetName(pResi),"GLN")==0||strcmp(ResidueGetName(pResi),"HSD")==0||strcmp(ResidueGetName(pResi),"HSE")==0){
          printf("We will flip residue %s%d%c to optimize hbond\n", ResidueGetChainName(pResi),ResidueGetPosInChain(pResi),ThreeLetterAAToOneLetterAA(ResidueGetName(pResi)));
          ProteinSiteAddCrystalRotamer(pStructure,i,j,resiTopos);
          ProteinSiteBuildFlippedCrystalRotamer(pStructure,i,j,resiTopos);
          ProteinSiteOptimizeRotamerHBondEnergy(pStructure,i,j);
        }
        else if(strcmp(ResidueGetName(pResi),"SER")==0 || strcmp(ResidueGetName(pResi),"THR")==0 || strcmp(ResidueGetName(pResi),"TYR")==0){
          printf("We will rotate hydroxyl group of residue %s%d%c to optimize hbond\n", ResidueGetChainName(pResi),ResidueGetPosInChain(pResi),ThreeLetterAAToOneLetterAA(ResidueGetName(pResi)));
          ProteinSiteAddCrystalRotamer(pStructure,i,j,resiTopos);
          ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
          ProteinSiteOptimizeRotamerHBondEnergy(pStructure,i,j);
        }
        if(TRUE){
          printf("We optimize side chain of residue %s%d%c\n", ResidueGetChainName(pResi),ResidueGetPosInChain(pResi),ThreeLetterAAToOneLetterAA(ResidueGetName(pResi)));
          ProteinSiteBuildWildtypeRotamersByBBdepRotLib(pStructure,i,j,pBBdepRotLib,atomParams,resiTopos);
          ProteinSiteAddCrystalRotamerByBBdepRotLib(pStructure,i,j,resiTopos,pBBdepRotLib);
          ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
          ProteinSiteOptimizeRotamerWithBBdepRotLib(pStructure,i,j,pBBdepRotLib);
          //ProteinSiteOptimizeRotamerLocallyWithBBdepRotLib(pStructure,i,j, 1.0,pBBdepRotLib);
        }
        ProteinSiteRemoveDesignSite(pStructure,i,j);
      }
    }
  }

  //output the repaired structure
  char modelfile[MAX_LENGTH_ONE_LINE_IN_FILE+1];
  if(pdbid!=NULL){sprintf(modelfile,"%s_Repair.pdb",pdbid);}
  else{strcpy(modelfile,"EvoEF_Repair.pdb");}
  FILE* pf=fopen(modelfile,"w");
  fprintf(pf,"REMARK EvoEF generated pdb file\n");
  fprintf(pf,"REMARK Output generated by EvoEF <RepairStructure>\n");
  StructureShowInPDBFormat(pStructure,TRUE,pf);
  fclose(pf);

  return Success;
}


int NOVA_WriteStructureToFile(Structure* pStructure, char* pdbfile){
  FILE* pf=fopen(pdbfile,"w");
  if(pf!=NULL){
    StructureShowInPDBFormat(pStructure,TRUE,pf);
    fclose(pf);
  }
  else{
    printf("failed to open file for writing structure coordinates\n");
    return IOError;
  }
  return Success;
}

int NOVA_AddHydrogen(Structure* pStructure, char* pdbid){
  //polar hydrogens are automatically added, so we just output the repaired structure
  char modelfile[MAX_LENGTH_ONE_LINE_IN_FILE+1];
  if(pdbid!=NULL){sprintf(modelfile,"%s_PolH.pdb",pdbid);}
  else{strcpy(modelfile,"EvoEF_PolH.pdb");}
  FILE* pf=fopen(modelfile,"w");
  fprintf(pf,"REMARK EvoEF generated pdb file\n");
  fprintf(pf,"REMARK Output generated by EvoEF <AddHydrogen>\n");
  StructureShowInPDBFormat(pStructure,FALSE,pf);
  fclose(pf);
  return Success;
}


int NOVA_OptimizeHydrogen(Structure* pStructure, AtomParamsSet* atomParams,ResiTopoSet* resiTopos, char* pdbid){
  for(int cycle=0; cycle<1; cycle++){
    printf("EvoEF Repairing PDB: optimization cycle %d ... \n",cycle+1);
    for(int i=0; i<StructureGetChainCount(pStructure); ++i){
      Chain* pChain = StructureGetChain(pStructure, i);
      for(int j=0; j<ChainGetResidueCount(pChain); j++){
        Residue* pResi = ChainGetResidue(pChain, j);
        if(strcmp(ResidueGetName(pResi),"SER")==0 || strcmp(ResidueGetName(pResi),"THR")==0 || strcmp(ResidueGetName(pResi),"TYR")==0){
          printf("We will rotate hydroxyl group of residue %s%d%c to optimize hbond\n", ResidueGetChainName(pResi),ResidueGetPosInChain(pResi),ThreeLetterAAToOneLetterAA(ResidueGetName(pResi)));
          ProteinSiteAddCrystalRotamer(pStructure,i,j,resiTopos);
          ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
          ProteinSiteOptimizeRotamerHBondEnergy(pStructure,i,j);
          ProteinSiteRemoveDesignSite(pStructure,i,j);
        }
      }
    }
  }

  //output the repaired structure
  char modelfile[MAX_LENGTH_ONE_LINE_IN_FILE+1];
  if(pdbid!=NULL){sprintf(modelfile,"%s_OptH.pdb",pdbid);}
  else{strcpy(modelfile,"EvoEF_OptH.pdb");}
  FILE* pf=fopen(modelfile,"w");
  fprintf(pf,"REMARK EvoEF generated pdb file\n");
  fprintf(pf,"REMARK Output generated by EvoEF <OptimizeHydrogen>\n");
  StructureShowInPDBFormat(pStructure,TRUE,pf);
  fclose(pf);

  return Success;
}


int NOVA_StructureComputeResidueInteractionWithFixedSurroundingResidues(Structure *pStructure, int chainIndex, int residueIndex){
  double energyTerms[MAX_EVOEF_ENERGY_TERM_NUM]={0};
  EnergyTermInitialize(energyTerms);
  // if the structure is composed of several chains, the residue position could be different in the whole structure from that in the separate chain
  StructureComputeResiduePosition(pStructure);
  Chain *pChainI=StructureGetChain(pStructure, chainIndex);
  Residue *pResIR= ChainGetResidue(pChainI, residueIndex);
  //step 1: find out residues within 5 angstroms to the design site of interest;
  int surroundingResiNum = 0;
  Residue **ppSurroundingResidues = NULL;
  for(int i = 0; i < pStructure->chainNum; i++){
    Chain *pChainI = pStructure->chains + i;
    for(int j = 0; j < pChainI->residueNum; j++){
      Residue *pResi2 = pChainI->residues + j;
      if(strcmp(pResIR->name, pResi2->name) == 0 && pResIR->posInChain == pResi2->posInChain){
        continue;
      }
      else if(AtomArrayCalcMinDistance(&pResIR->atoms, &pResi2->atoms)<ENERGY_DISTANCE_CUTOFF){
        surroundingResiNum++;
        ppSurroundingResidues = (Residue **)realloc(ppSurroundingResidues, sizeof(Residue*)*surroundingResiNum);
        ppSurroundingResidues[surroundingResiNum-1] = pResi2;
      }
    }
  }
  // calculate energy between residue IR and other residues
  EVOEF_AminoAcidReferenceEnergy(pResIR->name,energyTerms);
  EVOEF_EnergyResidueIntraEnergy(pResIR, energyTerms);
  for(int is = 0; is < surroundingResiNum; is++){
    Residue *pResIS = ppSurroundingResidues[is];
    if(strcmp(pResIR->chainName, pResIS->chainName) == 0 && pResIR->posInChain == pResIS->posInChain-1){
      EVOEF_EnergyResidueAndNextResidue(pResIR,pResIS,energyTerms);
    }
    else if(strcmp(pResIR->chainName, pResIS->chainName) == 0 && pResIR->posInChain == pResIS->posInChain+1){
      EVOEF_EnergyResidueAndNextResidue(pResIS,pResIR,energyTerms);
    }
    else{
      if(strcmp(pResIR->chainName, pResIS->chainName) == 0){
        EVOEF_EnergyResidueAndOtherResidueSameChain(pResIR,pResIS,energyTerms);
      }
      else{
        EVOEF_EnergyResidueAndOtherResidueDifferentChain(pResIR,pResIS,energyTerms);
      }
    }
  }

  if(TRUE){
    //for debug: energy details, not weighted
    printf("Energy details between residue %s%d%c and fixed surrounding residues:\n", ResidueGetChainName(pResIR),ResidueGetPosInChain(pResIR),ThreeLetterAAToOneLetterAA(ResidueGetName(pResIR)));
    printf("reference_ALA         =            %8.2f\n", energyTerms[ 1]);
    printf("reference_CYS         =            %8.2f\n", energyTerms[ 2]);
    printf("reference_ASP         =            %8.2f\n", energyTerms[ 3]);
    printf("reference_GLU         =            %8.2f\n", energyTerms[ 4]);
    printf("reference_PHE         =            %8.2f\n", energyTerms[ 5]);
    printf("reference_GLY         =            %8.2f\n", energyTerms[ 6]);
    printf("reference_HIS         =            %8.2f\n", energyTerms[ 7]);
    printf("reference_ILE         =            %8.2f\n", energyTerms[ 8]);
    printf("reference_LYS         =            %8.2f\n", energyTerms[ 9]);
    printf("reference_LEU         =            %8.2f\n", energyTerms[10]);
    printf("reference_MET         =            %8.2f\n", energyTerms[11]);
    printf("reference_ASN         =            %8.2f\n", energyTerms[12]);
    printf("reference_PRO         =            %8.2f\n", energyTerms[13]);
    printf("reference_GLN         =            %8.2f\n", energyTerms[14]);
    printf("reference_ARG         =            %8.2f\n", energyTerms[15]);
    printf("reference_SER         =            %8.2f\n", energyTerms[16]);
    printf("reference_THR         =            %8.2f\n", energyTerms[17]);
    printf("reference_VAL         =            %8.2f\n", energyTerms[18]);
    printf("reference_TRP         =            %8.2f\n", energyTerms[19]);
    printf("reference_TYR         =            %8.2f\n", energyTerms[20]);
    printf("intraR_vdwatt         =            %8.2f\n", energyTerms[21]);
    printf("intraR_vdwrep         =            %8.2f\n", energyTerms[22]);
    printf("intraR_electr         =            %8.2f\n", energyTerms[23]);
    printf("intraR_deslvP         =            %8.2f\n", energyTerms[24]);
    printf("intraR_deslvH         =            %8.2f\n", energyTerms[25]);
    printf("intraR_hbscbb_dis     =            %8.2f\n", energyTerms[26]);
    printf("intraR_hbscbb_the     =            %8.2f\n", energyTerms[27]);
    printf("intraR_hbscbb_phi     =            %8.2f\n", energyTerms[28]);
    printf("interS_vdwatt         =            %8.2f\n", energyTerms[31]);
    printf("interS_vdwrep         =            %8.2f\n", energyTerms[32]);
    printf("interS_electr         =            %8.2f\n", energyTerms[33]);
    printf("interS_deslvP         =            %8.2f\n", energyTerms[34]);
    printf("interS_deslvH         =            %8.2f\n", energyTerms[35]);
    printf("interS_hbbbbb_dis     =            %8.2f\n", energyTerms[41]);
    printf("interS_hbbbbb_the     =            %8.2f\n", energyTerms[42]);
    printf("interS_hbbbbb_phi     =            %8.2f\n", energyTerms[43]);
    printf("interS_hbscbb_dis     =            %8.2f\n", energyTerms[44]);
    printf("interS_hbscbb_the     =            %8.2f\n", energyTerms[45]);
    printf("interS_hbscbb_phi     =            %8.2f\n", energyTerms[46]);
    printf("interS_hbscsc_dis     =            %8.2f\n", energyTerms[47]);
    printf("interS_hbscsc_the     =            %8.2f\n", energyTerms[48]);
    printf("interS_hbscsc_phi     =            %8.2f\n", energyTerms[49]);
    printf("interD_vdwatt         =            %8.2f\n", energyTerms[51]);
    printf("interD_vdwrep         =            %8.2f\n", energyTerms[52]);
    printf("interD_electr         =            %8.2f\n", energyTerms[53]);
    printf("interD_deslvP         =            %8.2f\n", energyTerms[54]);
    printf("interD_deslvH         =            %8.2f\n", energyTerms[55]);
    printf("interD_hbbbbb_dis     =            %8.2f\n", energyTerms[61]);
    printf("interD_hbbbbb_the     =            %8.2f\n", energyTerms[62]);
    printf("interD_hbbbbb_phi     =            %8.2f\n", energyTerms[63]);
    printf("interD_hbscbb_dis     =            %8.2f\n", energyTerms[64]);
    printf("interD_hbscbb_the     =            %8.2f\n", energyTerms[65]);
    printf("interD_hbscbb_phi     =            %8.2f\n", energyTerms[66]);
    printf("interD_hbscsc_dis     =            %8.2f\n", energyTerms[67]);
    printf("interD_hbscsc_the     =            %8.2f\n", energyTerms[68]);
    printf("interD_hbscsc_phi     =            %8.2f\n", energyTerms[69]);
    //total energy: weighted
    EnergyTermWeighting(energyTerms);
    printf("----------------------------------------------------\n");
    printf("Total                 =            %8.2f\n\n", energyTerms[0]);
  }

  return Success;
}

//////////////////////////////////////////////////////////////////////////////
//The following are new program functions
//////////////////////////////////////////////////////////////////////////////
int NOVA_ComputeRotamersEnergy(Structure* pStructure,BBindRotamerLib* pBBindRotLib,AAppTable* pAAppTable,RamaTable* pRama,AtomParamsSet* atomParams,ResiTopoSet* resiTopos,char* pdbid){
  char energyfile[MAX_LENGTH_ONE_LINE_IN_FILE+1];
  sprintf(energyfile,"%s_rotenergy.txt",pdbid);
  FILE* fp=fopen(energyfile,"w");
  for(int i=0; i<StructureGetChainCount(pStructure); ++i){
    Chain* pChain = StructureGetChain(pStructure, i);
    if(ChainGetType(pChain)!=Type_Chain_Protein) continue;
    for(int j=0; j<ChainGetResidueCount(pChain); j++){
      Residue* pResi = ChainGetResidue(pChain, j);
      if(FLAG_PPI==TRUE){
        //check if residue is an interface residue
        for(int k=0;k<StructureGetChainCount(pStructure);k++){
          if(k==i) continue;
          Chain* pChainK=StructureGetChain(pStructure,k);
          for(int s=0;s<ChainGetResidueCount(pChainK);s++){
            Residue* pResiKS=ChainGetResidue(pChainK,s);
            if(AtomArrayCalcMinDistance(&pResi->atoms,&pResiKS->atoms)<ENERGY_DISTANCE_CUTOFF){
              ProteinSiteBuildAllRotamers(pStructure,i,j,pBBindRotLib,atomParams,resiTopos);
              if(FLAG_ADD_CRYSTAL_ROT==TRUE) ProteinSiteAddCrystalRotamer(pStructure,i,j,resiTopos);
              if(FLAG_EXPAND_HYDROXYL_ROT) ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
              ProteinSiteCalcRotamersEnergy(pStructure,pAAppTable,pRama,i,j,fp);
              ProteinSiteRemoveDesignSite(pStructure,i,j);
            }
          }
        }
      }
      else if(FLAG_ENZYME==TRUE || FLAG_PROT_LIG==TRUE){
        Residue* pSmallMol=NULL;
        StructureFindSmallMol(pStructure,&pSmallMol);
        if(AtomArrayCalcMinDistance(&pResi->atoms,&pSmallMol->atoms)<ENERGY_DISTANCE_CUTOFF){
          ProteinSiteBuildAllRotamers(pStructure,i,j,pBBindRotLib,atomParams,resiTopos);
          if(FLAG_ADD_CRYSTAL_ROT==TRUE) ProteinSiteAddCrystalRotamer(pStructure,i,j,resiTopos);
          if(FLAG_EXPAND_HYDROXYL_ROT) ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
          ProteinSiteCalcRotamersEnergy(pStructure,pAAppTable,pRama,i,j,fp);
          ProteinSiteRemoveDesignSite(pStructure,i,j);
        }
      }
      else{
        ProteinSiteBuildAllRotamers(pStructure,i,j,pBBindRotLib,atomParams,resiTopos);
        if(FLAG_ADD_CRYSTAL_ROT==TRUE) ProteinSiteAddCrystalRotamer(pStructure,i,j,resiTopos);
        if(FLAG_EXPAND_HYDROXYL_ROT) ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
        ProteinSiteCalcRotamersEnergy(pStructure,pAAppTable,pRama,i,j,fp);
        ProteinSiteRemoveDesignSite(pStructure,i,j);
      }
    }
  }
  fclose(fp);

  return Success;
}


int NOVA_ComputeWildtypeRotamersEnergy(Structure* pStructure,BBindRotamerLib* pBBindRotLib,AAppTable* pAAppTable,RamaTable* pRama,AtomParamsSet* atomParams,ResiTopoSet* resiTopos, char* pdbid){
  char energyfile[MAX_LENGTH_ONE_LINE_IN_FILE+1];
  sprintf(energyfile,"%s_rotenergy.txt",pdbid);
  FILE* fp=fopen(energyfile,"w");
  for(int i=0; i<StructureGetChainCount(pStructure); ++i){
    Chain* pChain = StructureGetChain(pStructure, i);
    if(ChainGetType(pChain)!=Type_Chain_Protein) continue;
    for(int j=0; j<ChainGetResidueCount(pChain); j++){
      Residue* pResi = ChainGetResidue(pChain, j);
      if(FLAG_PPI==TRUE){
        //check if residue is an interface residue
        for(int k=0;k<StructureGetChainCount(pStructure);k++){
          if(k==i) continue;
          Chain* pChainK=StructureGetChain(pStructure,k);
          for(int s=0;s<ChainGetResidueCount(pChainK);s++){
            Residue* pResiKS=ChainGetResidue(pChainK,s);
            if(AtomArrayCalcMinDistance(&pResi->atoms,&pResiKS->atoms)<ENERGY_DISTANCE_CUTOFF){
              ProteinSiteBuildWildtypeRotamers(pStructure,i,j,pBBindRotLib,atomParams,resiTopos);
              if(FLAG_ADD_CRYSTAL_ROT==TRUE) ProteinSiteAddCrystalRotamer(pStructure,i,j,resiTopos);
              if(FLAG_EXPAND_HYDROXYL_ROT) ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
              ProteinSiteCalcRotamersEnergy(pStructure,pAAppTable,pRama,i,j,fp);
              ProteinSiteRemoveDesignSite(pStructure,i,j);
            }
          }
        }
      }
      else if(FLAG_ENZYME==TRUE || FLAG_PROT_LIG==TRUE){
        Residue* pSmallMol=NULL;
        StructureFindSmallMol(pStructure,&pSmallMol);
        if(AtomArrayCalcMinDistance(&pResi->atoms,&pSmallMol->atoms)<ENERGY_DISTANCE_CUTOFF){
          ProteinSiteBuildWildtypeRotamers(pStructure,i,j,pBBindRotLib,atomParams,resiTopos);
          if(FLAG_ADD_CRYSTAL_ROT==TRUE) ProteinSiteAddCrystalRotamer(pStructure,i,j,resiTopos);
          if(FLAG_EXPAND_HYDROXYL_ROT) ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
          ProteinSiteCalcRotamersEnergy(pStructure,pAAppTable,pRama,i,j,fp);
          ProteinSiteRemoveDesignSite(pStructure,i,j);
        }
      }
      else{
        ProteinSiteBuildWildtypeRotamers(pStructure,i,j,pBBindRotLib,atomParams,resiTopos);
        if(FLAG_ADD_CRYSTAL_ROT==TRUE) ProteinSiteAddCrystalRotamer(pStructure,i,j,resiTopos);
        if(FLAG_EXPAND_HYDROXYL_ROT) ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
        ProteinSiteCalcRotamersEnergy(pStructure,pAAppTable,pRama,i,j,fp);
        ProteinSiteRemoveDesignSite(pStructure,i,j);
      }
    }
  }
  fclose(fp);

  return Success;
}


int NOVA_StructureFindInterfaceResidues(Structure *pStructure, double cutoff,char* outputfile){
  if(pStructure->chainNum < 2){
    printf("there is only one chain in the whole structure, no protein-protein interface found\n");
    exit(ValueError);
  }

  IntArray* chainArrays=(IntArray*)malloc(sizeof(IntArray)*StructureGetChainCount(pStructure));
  for(int i=0;i<StructureGetChainCount(pStructure);i++){
    Chain* pChainI=StructureGetChain(pStructure,i);
    IntArrayCreate(&chainArrays[i],pChainI->residueNum);
    for(int j=0;j<pChainI->residueNum;j++){
      IntArraySet(&chainArrays[i],j,0);
    }
  }

  for(int i = 0; i < pStructure->chainNum; i++){
    Chain *pChainI = StructureGetChain(pStructure, i);
    for(int k = i+1; k < pStructure->chainNum; k++){
      Chain *pChainK = StructureGetChain(pStructure, k);
      for(int j = 0; j < pChainI->residueNum; j++){
        Residue* pResiIJ = ChainGetResidue(pChainI, j);
        //if(IntArrayGet(&chainArrays[i],j)==1)continue;
        for(int s = 0; s < pChainK->residueNum; s++){
          Residue* pResiKS = ChainGetResidue(pChainK, s);
          //if(IntArrayGet(&chainArrays[k],s)==1)continue;
          if(AtomArrayCalcMinDistance(&pResiIJ->atoms, &pResiKS->atoms) < cutoff){
            IntArraySet(&chainArrays[i], j, 1);
            IntArraySet(&chainArrays[k], s, 1);
          }
        }
      }
    }
  }

  FILE* fout=fopen(outputfile,"w");
  printf("Interface residues: \n");
  for(int j=0;j<StructureGetChainCount(pStructure);j++){
    Chain* pChain=StructureGetChain(pStructure,j);
    if(ChainGetType(pChain)==Type_Chain_Protein || ChainGetType(pChain)==Type_Chain_Nucleotide){
      for(int i=0; i<ChainGetResidueCount(pChain); i++){
        Residue* pResidue=ChainGetResidue(pChain,i);
        if(IntArrayGet(&chainArrays[j],i)==1){
          printf("%c",ThreeLetterAAToOneLetterAA(ResidueGetName(pResidue)));
          fprintf(fout,"%c",ThreeLetterAAToOneLetterAA(ResidueGetName(pResidue)));
        }
        else{
          printf("-");
          fprintf(fout,"-");
        }
      }
      printf("\n");
      fprintf(fout,"\n");
    }
  }

  fclose(fout);

  return Success;
}


int StructureCalcPhiPsi(Structure* pStructure){
  for(int i=0;i<StructureGetChainCount(pStructure);i++){
    Chain* pChain=StructureGetChain(pStructure,i);
    if(ChainGetType(pChain)!=Type_Chain_Protein) continue;
    for(int j=0;j<ChainGetResidueCount(pChain);j++){
      Residue* pResi0=ChainGetResidue(pChain,j-1);
      Residue* pResi1=ChainGetResidue(pChain,j);
      Residue* pResi2=ChainGetResidue(pChain,j+1);

      Atom* pC0=NULL;
      Atom* pN1=ResidueGetAtomByName(pResi1,"N");
      Atom* pCA1=ResidueGetAtomByName(pResi1,"CA");
      Atom* pC1=ResidueGetAtomByName(pResi1,"C");
      Atom* pN2=NULL;

      double phi=0.0;
      double psi=0.0;
      if(pResi0==NULL){
        phi=-60.0;
        pC0=NULL;
      }
      else{
        pC0=ResidueGetAtomByName(pResi0,"C");
      }

      if(pResi2==NULL){
        psi=60.0;
        pN2=NULL;
      }
      else{
        pN2=ResidueGetAtomByName(pResi2,"N");
      }

      if(pC0!=NULL){
        double dist_C0N1=XYZDistance(&pC0->xyz,&pN1->xyz);
        if(dist_C0N1<1.45 && dist_C0N1>1.25){
          phi=RadToDeg(GetTorsionAngle(&pC0->xyz,&pN1->xyz,&pCA1->xyz,&pC1->xyz));
        }
        else{
          phi=-60.0;
        }
      }
      
      if(pN2!=NULL){
        double dist_C1N2=XYZDistance(&pC1->xyz,&pN2->xyz);
        if(dist_C1N2<1.45 && dist_C1N2>1.25){
          psi=RadToDeg(GetTorsionAngle(&pN1->xyz,&pCA1->xyz,&pC1->xyz,&pN2->xyz));
        }
        else{
          psi=60.0;
        }
      }

      //assign phi and psi values to residue1
      pResi1->phipsi[0]=phi;
      pResi1->phipsi[1]=psi;
    }
  }

  return Success;
}

int NOVA_StructureShowPhiPsi(Structure* pStructure,char* phipsifile){
  FILE* fout=NULL;
  if(phipsifile==NULL) fout=stdout;
  else fout=fopen(phipsifile,"w");
  fprintf(fout,"#phi-psi angles of protein residues: \n");
  for(int i=0;i<StructureGetChainCount(pStructure);i++){
    Chain* pChain=StructureGetChain(pStructure,i);
    if(ChainGetType(pChain)!=Type_Chain_Protein) continue;
    for(int j=0;j<ChainGetResidueCount(pChain);j++){
      Residue* pResi1=ChainGetResidue(pChain,j);
      fprintf(fout,"%s %s %4d %8.3f %8.3f\n",ResidueGetChainName(pResi1),ResidueGetName(pResi1),ResidueGetPosInChain(pResi1),pResi1->phipsi[0],pResi1->phipsi[1]);
    }
  }
  if(fout != stdout) fclose(fout);

  return Success;
}


int NOVA_ComputeRotamersEnergyByBBdepRotLib(Structure* pStructure,BBdepRotamerLib* pBBdepRotLib,AAppTable* pAAppTable,RamaTable* pRama,AtomParamsSet* atomParams,ResiTopoSet* resiTopos,char* pdbid){
  char energyfile[MAX_LENGTH_ONE_LINE_IN_FILE+1];
  sprintf(energyfile,"%s_rotenergy.txt",pdbid);
  FILE* fp=fopen(energyfile,"w");
  for(int i=0; i<StructureGetChainCount(pStructure); ++i){
    Chain* pChain = StructureGetChain(pStructure, i);
    if(ChainGetType(pChain)!=Type_Chain_Protein) continue;
    for(int j=0; j<ChainGetResidueCount(pChain); j++){
      Residue* pResi = ChainGetResidue(pChain, j);
      if(FLAG_PPI==TRUE){
        //check if residue is an interface residue
        for(int k=0;k<StructureGetChainCount(pStructure);k++){
          if(k==i) continue;
          Chain* pChainK=StructureGetChain(pStructure,k);
          for(int s=0;s<ChainGetResidueCount(pChainK);s++){
            Residue* pResiKS=ChainGetResidue(pChainK,s);
            if(AtomArrayCalcMinDistance(&pResi->atoms,&pResiKS->atoms)<ENERGY_DISTANCE_CUTOFF){
              ProteinSiteBuildAllRotamersByBBdepRotLib(pStructure,i,j,pBBdepRotLib,atomParams,resiTopos);
              if(FLAG_ADD_CRYSTAL_ROT==TRUE) ProteinSiteAddCrystalRotamerByBBdepRotLib(pStructure,i,j,resiTopos,pBBdepRotLib);
              if(FLAG_EXPAND_HYDROXYL_ROT) ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
              ProteinSiteCalcRotamersEnergy(pStructure,pAAppTable,pRama,i,j,fp);
              ProteinSiteRemoveDesignSite(pStructure,i,j);
            }
          }
        }
      }
      else if(FLAG_ENZYME==TRUE || FLAG_PROT_LIG==TRUE){
        Residue* pSmallMol=NULL;
        StructureFindSmallMol(pStructure,&pSmallMol);
        if(AtomArrayCalcMinDistance(&pResi->atoms,&pSmallMol->atoms)<ENERGY_DISTANCE_CUTOFF){
          ProteinSiteBuildAllRotamersByBBdepRotLib(pStructure,i,j,pBBdepRotLib,atomParams,resiTopos);
          if(FLAG_ADD_CRYSTAL_ROT==TRUE) ProteinSiteAddCrystalRotamerByBBdepRotLib(pStructure,i,j,resiTopos,pBBdepRotLib);
          if(FLAG_EXPAND_HYDROXYL_ROT) ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
          ProteinSiteCalcRotamersEnergy(pStructure,pAAppTable,pRama,i,j,fp);
          ProteinSiteRemoveDesignSite(pStructure,i,j);
        }
      }
      else{
        ProteinSiteBuildAllRotamersByBBdepRotLib(pStructure,i,j,pBBdepRotLib,atomParams,resiTopos);
        if(FLAG_ADD_CRYSTAL_ROT==TRUE) ProteinSiteAddCrystalRotamerByBBdepRotLib(pStructure,i,j,resiTopos,pBBdepRotLib);
        if(FLAG_EXPAND_HYDROXYL_ROT) ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
        ProteinSiteCalcRotamersEnergy(pStructure,pAAppTable,pRama,i,j,fp);
        ProteinSiteRemoveDesignSite(pStructure,i,j);
      }
    }
  }
  fclose(fp);

  return Success;
}


int NOVA_ComputeWildtypeRotamersEnergyByBBdepRotLib(Structure* pStructure,BBdepRotamerLib* pBBdepRotLib,AAppTable* pAAppTable,RamaTable* pRama,AtomParamsSet* atomParams,ResiTopoSet* resiTopos,char* pdbid){
  char energyfile[MAX_LENGTH_ONE_LINE_IN_FILE+1];
  sprintf(energyfile,"%s_rotenergy.txt",pdbid);
  FILE* fp=fopen(energyfile,"w");
  for(int i=0; i<StructureGetChainCount(pStructure); ++i){
    Chain* pChain = StructureGetChain(pStructure, i);
    if(ChainGetType(pChain)!=Type_Chain_Protein) continue;
    for(int j=0; j<ChainGetResidueCount(pChain); j++){
      Residue* pResi = ChainGetResidue(pChain, j);
      if(FLAG_PPI==TRUE){
        //check if residue is an interface residue
        for(int k=0;k<StructureGetChainCount(pStructure);k++){
          if(k==i) continue;
          Chain* pChainK=StructureGetChain(pStructure,k);
          for(int s=0;s<ChainGetResidueCount(pChainK);s++){
            Residue* pResiKS=ChainGetResidue(pChainK,s);
            if(AtomArrayCalcMinDistance(&pResi->atoms,&pResiKS->atoms)<ENERGY_DISTANCE_CUTOFF){
              ProteinSiteBuildWildtypeRotamersByBBdepRotLib(pStructure,i,j,pBBdepRotLib,atomParams,resiTopos);
              if(FLAG_ADD_CRYSTAL_ROT==TRUE) ProteinSiteAddCrystalRotamerByBBdepRotLib(pStructure,i,j,resiTopos,pBBdepRotLib);
              if(FLAG_EXPAND_HYDROXYL_ROT) ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
              ProteinSiteCalcRotamersEnergy(pStructure,pAAppTable,pRama,i,j,fp);
              ProteinSiteRemoveDesignSite(pStructure,i,j);
            }
          }
        }
      }
      else if(FLAG_ENZYME==TRUE || FLAG_PROT_LIG==TRUE){
        Residue* pSmallMol=NULL;
        StructureFindSmallMol(pStructure,&pSmallMol);
        if(AtomArrayCalcMinDistance(&pResi->atoms,&pSmallMol->atoms)<ENERGY_DISTANCE_CUTOFF){
          ProteinSiteBuildWildtypeRotamersByBBdepRotLib(pStructure,i,j,pBBdepRotLib,atomParams,resiTopos);
          if(FLAG_ADD_CRYSTAL_ROT==TRUE) ProteinSiteAddCrystalRotamerByBBdepRotLib(pStructure,i,j,resiTopos,pBBdepRotLib);
          if(FLAG_EXPAND_HYDROXYL_ROT) ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
          ProteinSiteCalcRotamersEnergy(pStructure,pAAppTable,pRama,i,j,fp);
          ProteinSiteRemoveDesignSite(pStructure,i,j);
        }
      }
      else{
        ProteinSiteBuildWildtypeRotamersByBBdepRotLib(pStructure,i,j,pBBdepRotLib,atomParams,resiTopos);
        if(FLAG_ADD_CRYSTAL_ROT==TRUE) ProteinSiteAddCrystalRotamerByBBdepRotLib(pStructure,i,j,resiTopos,pBBdepRotLib);
        if(FLAG_EXPAND_HYDROXYL_ROT) ProteinSiteExpandHydroxylRotamers(pStructure,i,j,resiTopos);
        ProteinSiteCalcRotamersEnergy(pStructure,pAAppTable,pRama,i,j,fp);
        ProteinSiteRemoveDesignSite(pStructure,i,j);
      }
    }
  }
  fclose(fp);

  return Success;
}



int NOVA_CheckRotamerInBBindRotLib(Structure* pStructure,BBindRotamerLib* pBBindRotLib,ResiTopoSet* pTopos,double cutoff,char* pdbid){
  char FILE_TORSION[MAX_LENGTH_FILE_NAME+1];
  sprintf(FILE_TORSION,"%s_torsionrecover.txt",pdbid);
  FILE* pf=fopen(FILE_TORSION,"w");  for(int i=0;i<StructureGetChainCount(pStructure);i++){
    Chain* pChain=StructureGetChain(pStructure,i);
    for(int j=0;j<ChainGetResidueCount(pChain);j++){
      Residue* pResidue=ChainGetResidue(pChain,j);
      if(strcmp(ResidueGetName(pResidue),"ALA")!=0 && strcmp(ResidueGetName(pResidue),"GLY")!=0){
        BOOL result=ProteinSiteCheckCrystalRotamerInBBindRotLib(pStructure,i,j,pTopos,pBBindRotLib,cutoff);
        if(result==TRUE){
          fprintf(pf,"%s %s 1\n",ChainGetName(pChain),ResidueGetName(pResidue));
        }
        else{
          fprintf(pf,"%s %s 0\n",ChainGetName(pChain),ResidueGetName(pResidue));
        }
      }
      else{
        fprintf(pf,"%s %s 1\n",ChainGetName(pChain),ResidueGetName(pResidue));
      }
    }
  }
  fclose(pf);
  return Success;
}


int NOVA_CheckRotamerInBBdepRotLib(Structure* pStructure,BBdepRotamerLib* pBBdepRotLib,ResiTopoSet* pTopos,double cutoff,char* pdbid){
  char FILE_TORSION[MAX_LENGTH_FILE_NAME+1];
  sprintf(FILE_TORSION,"%s_torsionrecover.txt",pdbid);
  FILE* pf=fopen(FILE_TORSION,"w");
  for(int i=0;i<StructureGetChainCount(pStructure);i++){
    Chain* pChain=StructureGetChain(pStructure,i);
    for(int j=0;j<ChainGetResidueCount(pChain);j++){
      Residue* pResidue=ChainGetResidue(pChain,j);
      if(strcmp(ResidueGetName(pResidue),"ALA")!=0 && strcmp(ResidueGetName(pResidue),"GLY")!=0){
        BOOL result=ProteinSiteCheckCrystalRotamerInBBdepRotLib(pStructure,i,j,pTopos,pBBdepRotLib,cutoff);
        if(result==TRUE){
          fprintf(pf,"%s %s 1\n",ChainGetName(pChain),ResidueGetName(pResidue));
        }
        else{
          fprintf(pf,"%s %s 0\n",ChainGetName(pChain),ResidueGetName(pResidue));
        }
      }
      else{
        fprintf(pf,"%s %s 1\n",ChainGetName(pChain),ResidueGetName(pResidue));
      }
    }
  }
  fclose(pf);
  return Success;
}


int NOVA_GetResiMinRmsdRotFromLab(Structure* pStructure,char* pdbid){
  char FILE_RMSD[MAX_LENGTH_FILE_NAME+1];
  sprintf(FILE_RMSD,"%s_minrmsd.txt",pdbid);
  FILE* pf=fopen(FILE_RMSD,"w");
  for(int i=0;i<StructureGetChainCount(pStructure);i++){
    Chain* pChain=StructureGetChain(pStructure,i);
    for(int j=0;j<ChainGetResidueCount(pChain);j++){
      Residue* pResidue=ChainGetResidue(pChain,j);
      DesignSite* pSite=StructureFindDesignSite(pStructure,i,j);
      if(pSite!=NULL){
        double minRMSD=1e8;
        RotamerSet* pSet=DesignSiteGetRotamers(pSite);
        for(int k=0;k<RotamerSetGetCount(pSet);k++){
          Rotamer* pRotamer=RotamerSetGet(pSet,k);
          RotamerRestore(pRotamer,pSet);
          double rmsd = RotamerAndResidueSidechainRMSD(pRotamer,pResidue);
          if(RotamerIsSymmetricalCheck(pRotamer)==TRUE){
            Rotamer tempRot;
            RotamerCreate(&tempRot);
            SymmetricalRotamerGenerate(&tempRot,pRotamer);
            double rmsd2=RotamerAndResidueSidechainRMSD(&tempRot,pResidue);
            if(rmsd2<rmsd) rmsd=rmsd2;
            RotamerDestroy(&tempRot);
          }
          if(rmsd<minRMSD) minRMSD=rmsd;
          RotamerExtract(pRotamer);
        }
        fprintf(pf,"%s %s %f\n",ChainGetName(pChain),ResidueGetName(pResidue),minRMSD);
      }
    }
  }
  fclose(pf);
  return Success;
}
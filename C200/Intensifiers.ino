void intensifier1Operation(){
  if(SUB_STATE1 != PREV1){
    timer[2] = 0;
    deltasLow.clear();
    DO_HYD_XV460_DCV1_A = false;
    DO_HYD_XV463_DCV1_B = false;
    peakPsi1A = 0;
    peakPsi1B = 0;
    stateHistory1 = stateHistory1 + String(SUB_STATE1);
    PREV1 = SUB_STATE1;
    return;
  }
  switch(SUB_STATE1){
    case OFF:
      DO_HYD_XV460_DCV1_A = false;
      DO_HYD_XV463_DCV1_B = false;
    break;

    case START:
      if(!timer[2]){timer[2] = millis();DO_HYD_XV463_DCV1_B = true;stateHistory1 = stateHistory1 + "-";}
      if(millis() - timer[2] > 3000 && timer[2]){
        DO_HYD_XV463_DCV1_B = false;
        SUB_STATE1 = DEADHEAD1;//(!deadHeadPsi1A || !deadHeadPsi1B) ? DEADHEAD1 : SIDE_A;
      }
      
    break;

    case DEADHEAD1:
      if(!timer[2]){timer[2] = millis();DO_HYD_XV460_DCV1_A = true;stateHistory1 = stateHistory1 + "+";}
      if(millis() - timer[2] > 500 && timer[2]){
        if(AI_HYD_psig_PT467_HydraulicInlet1 > peakPsi1A){peakPsi1A = AI_HYD_psig_PT467_HydraulicInlet1;}
      }
      if(millis() - timer[2] > 3000 && timer[2]){
        deadHeadPsi1A = peakPsi1A;
        DO_HYD_XV460_DCV1_A = false;
        SUB_STATE1 = DEADHEAD2;
      }
    break;

    case DEADHEAD2:
      if(!timer[2]){timer[2] = millis();DO_HYD_XV463_DCV1_B = true;stateHistory1 = stateHistory1 + "-";}
      if(millis() - timer[2] > 500 && timer[2]){
        if(AI_HYD_psig_PT467_HydraulicInlet1 > peakPsi1B){peakPsi1B = AI_HYD_psig_PT467_HydraulicInlet1;}
      }
      if(millis() - timer[2] > 3000 && timer[2]){
        deadHeadPsi1B = peakPsi1B;
        DO_HYD_XV463_DCV1_B = false;
        SUB_STATE1 = SIDE_A;
      }
    break;

    case SIDE_A:
      if(!timer[2]){ timer[2] = millis(); DO_HYD_XV460_DCV1_A = true; stateHistory1 = stateHistory1 + "+";}
      if(millis() - timer[2] > 500 && timer[2]){
        if(AI_HYD_psig_PT467_HydraulicInlet1 > peakPsi1A){ peakPsi1A = AI_HYD_psig_PT467_HydraulicInlet1; }

        if(AI_HYD_psig_PT467_HydraulicInlet1 > 200){
          if(!prev1A){ prev1A = AI_HYD_psig_PT467_HydraulicInlet1;}
          else if(prev1A < AI_HYD_psig_PT467_HydraulicInlet1){ deltasLow.addValue(AI_HYD_psig_PT467_HydraulicInlet1 - prev1A); }
        }

        if(deltasLow.getCount() > 4 && deltasLow.getValue(deltasLow.getCount()-1) > stdDevMult1A*deltasLow.getStandardDeviation() + deltasLow.getFastAverage()){
          stdDevMult1A = stdDevMult1A + 0.01;
          DO_HYD_XV460_DCV1_A = false;
        }
        else if(peakPsi1A > deadHeadPsi1A - deadHeadDelta1A){
          DO_HYD_XV460_DCV1_A = false;
          stdDevMult1A = (stdDevMult1A <= 0.5)?0.5:(stdDevMult1A - 0.05);
        }
      }
      if((millis() - timer[2] > 30000 && timer[2])){STATE = FAULT; faultString = faultString + "|1A Timeout|";}
      
      if(millis() - timer[2] > switchingTime1A-550 && timer[2] && !DO_HYD_XV460_DCV1_A){//check if minimum time has passed
        SUB_STATE1 = SIDE_B;
        lowCycleCnt++; //reached end of cycle time, switch sides 
      }
    break;

    case SIDE_B:
      if(!timer[2]){ timer[2] = millis(); DO_HYD_XV463_DCV1_B = true; stateHistory1 = stateHistory1 + "-";}
      if(millis() - timer[2] > 500 && timer[2]){
        if(AI_HYD_psig_PT467_HydraulicInlet1 > peakPsi1B){peakPsi1B = AI_HYD_psig_PT467_HydraulicInlet1; }

        if(AI_HYD_psig_PT467_HydraulicInlet1 > 200){
          if(!prev1B){ prev1B = AI_HYD_psig_PT467_HydraulicInlet1;}
          else if(prev1B < AI_HYD_psig_PT467_HydraulicInlet1){ deltasLow.addValue(AI_HYD_psig_PT467_HydraulicInlet1 - prev1B); }
        }

        if(deltasLow.getCount() > 4 && deltasLow.getValue(deltasLow.getCount()-1) > stdDevMult1B*deltasLow.getStandardDeviation() + deltasLow.getFastAverage()){
          stdDevMult1B = stdDevMult1B + 0.01;
          DO_HYD_XV463_DCV1_B = false;
        }
        else if(peakPsi1B > deadHeadPsi1B - deadHeadDelta1B){
          DO_HYD_XV463_DCV1_B = false;
          stdDevMult1B = (stdDevMult1B <= 0.5)?0.5:(stdDevMult1B - 0.05);
        }
      }
      if((millis() - timer[2] > 30000 && timer[2])){STATE = FAULT; faultString = faultString + "|1B Timeout|";}

      if(millis() - timer[2] > switchingTime1B-550 && timer[2] && !DO_HYD_XV463_DCV1_B){//check if minimum time has passed
        SUB_STATE1 = SIDE_A;
        lowCycleCnt++; //reached end of cycle time, switch sides 
      }
    break;

    case PAUSE:
      if(!timer[2]){ timer[2] = millis(); DO_HYD_XV460_DCV1_A = false; DO_HYD_XV463_DCV1_B = false;}
      
      if(manualPause){break;}
      for(int i = 0; i < PTsize;i++){
        if(PTdata[i].overPressure){ break; }
      }
      if(millis() - timer[2] > 5000 && timer[2]){ SUB_STATE1 = SIDE_A; }
      
    break;

    default:
    break;
  }
}





void intensifier2Operation(){
  if(SUB_STATE2 != PREV2){
    timer[3] = 0;
    deltasHigh.clear();
    DO_HYD_XV554_DCV2_A = false;
    DO_HYD_XV557_DCV2_B = false;
    peakPsi2A = 0;
    peakPsi2B = 0;
    stateHistory2 = stateHistory2 + String(SUB_STATE2);
    PREV2 = SUB_STATE2;
    return;
  }
  switch(SUB_STATE2){
    case OFF:
      DO_HYD_XV554_DCV2_A = false;
      DO_HYD_XV557_DCV2_B = false;
    break;

    case START:
      if(!timer[3]){timer[3] = millis();DO_HYD_XV557_DCV2_B = true;stateHistory2 = stateHistory2 + "-";}
      if(millis() - timer[3] > 3000 && timer[3]){
        DO_HYD_XV557_DCV2_B = false;
        SUB_STATE2 = DEADHEAD1;//(!deadHeadPsi2A || !deadHeadPsi2B) ? DEADHEAD1 : SIDE_A;
      }
      
    break;

    case DEADHEAD1:
      if(!timer[3]){timer[3] = millis();DO_HYD_XV554_DCV2_A = true;stateHistory2 = stateHistory2 + "+";}
      if(millis() - timer[3] > 500 && timer[3]){
        if(AI_HYD_psig_PT561_HydraulicInlet2 > peakPsi2A){peakPsi2A = AI_HYD_psig_PT561_HydraulicInlet2;}
      }
      if(millis() - timer[3] > 3000 && timer[3]){
        deadHeadPsi2A = peakPsi2A;
        DO_HYD_XV554_DCV2_A = false;
        SUB_STATE2 = DEADHEAD2;
      }
    break;

    case DEADHEAD2:
      if(!timer[3]){timer[3] = millis();DO_HYD_XV557_DCV2_B = true;stateHistory2 = stateHistory2 + "-";}
      if(millis() - timer[3] > 500 && timer[3]){
        if(AI_HYD_psig_PT561_HydraulicInlet2 > peakPsi2B){peakPsi2B = AI_HYD_psig_PT561_HydraulicInlet2;}
      }
      if(millis() - timer[3] > 3000 && timer[3]){
        deadHeadPsi2B = peakPsi2B;
        DO_HYD_XV557_DCV2_B = false;
        SUB_STATE2 = SIDE_A;
      }
    break;

    case SIDE_A:
      if(!timer[3]){ timer[3] = millis(); DO_HYD_XV554_DCV2_A = true; stateHistory2 = stateHistory2 + "+";}
      if(millis() - timer[3] > 500 && timer[3]){
        if(AI_HYD_psig_PT561_HydraulicInlet2 > peakPsi2A){peakPsi2A = AI_HYD_psig_PT561_HydraulicInlet2; }

        if(AI_HYD_psig_PT561_HydraulicInlet2 > 200){
          if(!prev2A){ prev2A = AI_HYD_psig_PT561_HydraulicInlet2;}
          else if(prev2A < AI_HYD_psig_PT561_HydraulicInlet2){ deltasHigh.addValue(AI_HYD_psig_PT561_HydraulicInlet2 - prev2A); }
        }
        if(deltasHigh.getCount() > 4 && deltasHigh.getValue(deltasHigh.getCount()-1) > stdDevMult2A*deltasHigh.getStandardDeviation() + deltasHigh.getFastAverage()){
          stdDevMult2A = stdDevMult2A + 0.01;
          DO_HYD_XV554_DCV2_A = false;
        }
        else if(peakPsi2A > deadHeadPsi2A - deadHeadDelta2A){
          DO_HYD_XV554_DCV2_A = false;
          stdDevMult2A = (stdDevMult2A <= 0.5)?0.5:(stdDevMult2A - 0.05);
        }
        
      }
      if((millis() - timer[3] > 30000 && timer[3])){STATE = FAULT; faultString = faultString + "|2A Timeout|";}
      
      if(millis() - timer[3] > switchingTime2A-550 && timer[3] && !DO_HYD_XV554_DCV2_A){//check if minimum time has passed
        SUB_STATE2 = SIDE_B;
        highCycleCnt++; //reached end of cycle time, switch sides 
      }
    break;

    case SIDE_B:
      if(!timer[3]){ timer[3] = millis(); DO_HYD_XV557_DCV2_B = true; stateHistory2 = stateHistory2 + "-";}
      if(millis() - timer[3] > 500 && timer[3]){
        if(AI_HYD_psig_PT561_HydraulicInlet2 > peakPsi2B){peakPsi2B = AI_HYD_psig_PT561_HydraulicInlet2; }

        if(AI_HYD_psig_PT561_HydraulicInlet2 > 200){
          if(!prev2B){ prev2B = AI_HYD_psig_PT561_HydraulicInlet2;}
          else if(prev2B < AI_HYD_psig_PT561_HydraulicInlet2){ deltasHigh.addValue(AI_HYD_psig_PT561_HydraulicInlet2 - prev2B); }
        }

        if(deltasHigh.getCount() > 4 && deltasHigh.getValue(deltasHigh.getCount()-1) > stdDevMult2B*deltasHigh.getStandardDeviation() + deltasHigh.getFastAverage()){
          stdDevMult2B = stdDevMult2B + 0.01;
          DO_HYD_XV557_DCV2_B = false;
        }
        else if(peakPsi2B > deadHeadPsi2B - deadHeadDelta2B){
          DO_HYD_XV557_DCV2_B = false;
          stdDevMult2B = (stdDevMult2B <= 0.5)?0.5:(stdDevMult2B - 0.05);
        }
      }

      if((millis() - timer[3] > 30000 && timer[3])){STATE = FAULT; faultString = faultString + "|2B Timeout|";}
      
      if(millis() - timer[3] > switchingTime2B-550 && timer[3] && !DO_HYD_XV557_DCV2_B){//check if minimum time has passed
        SUB_STATE2 = SIDE_A;
        highCycleCnt++; //reached end of cycle time, switch sides 
      }
    break;

    case PAUSE:
      if(!timer[3]){ timer[3] = millis(); DO_HYD_XV554_DCV2_A = false; DO_HYD_XV557_DCV2_B = false;}
      
      if(manualPause){break;}
      for(int i = 0; i < PTsize;i++){
        if(PTdata[i].overPressure){ break; }
      }
      if(millis() - timer[3] > 5000 && timer[3]){ SUB_STATE2 = SIDE_A; }
      
    break;

    default:
    break;
  }
}















void intensifier2Operation_OLD(){
  if(SUB_STATE2 != PREV2){
    timer[3] = 0;
    deltasHigh.clear();
    DO_HYD_XV554_DCV2_A = false;
    DO_HYD_XV557_DCV2_B = false;
    peakPsi2A = 0;
    peakPsi2B = 0;
    stateHistory2 = stateHistory2 + String(SUB_STATE2);
    PREV2 = SUB_STATE2;
    return;
  }
  switch(SUB_STATE2){
    case OFF:
      DO_HYD_XV554_DCV2_A = false;
      DO_HYD_XV557_DCV2_B = false;
    break;

    case START:
      if(!timer[3]){timer[3] = millis();DO_HYD_XV557_DCV2_B = true;stateHistory2 = stateHistory2 + "-";}
      if(millis() - timer[3] > 3000 && timer[3]){
        DO_HYD_XV557_DCV2_B = false;
        SUB_STATE2 = DEADHEAD1;//(!deadHeadPsi2A || !deadHeadPsi2B) ? DEADHEAD1 : SIDE_A;
      }
      
    break;

    case DEADHEAD1:
      if(!timer[3]){timer[3] = millis();DO_HYD_XV554_DCV2_A = true;stateHistory2 = stateHistory2 + "+";}
      if(millis() - timer[3] > 500 && timer[3]){
        if(AI_HYD_psig_PT561_HydraulicInlet2 > peakPsi2A){peakPsi2A = AI_HYD_psig_PT561_HydraulicInlet2;}
      }
      if(millis() - timer[3] > 3000 && timer[3]){
        deadHeadPsi2A = peakPsi2A;
        DO_HYD_XV554_DCV2_A = false;
        SUB_STATE2 = DEADHEAD2;
      }
    break;

    case DEADHEAD2:
      if(!timer[3]){timer[3] = millis();DO_HYD_XV557_DCV2_B = true;stateHistory2 = stateHistory2 + "-";}
      if(millis() - timer[3] > 500 && timer[3]){
        if(AI_HYD_psig_PT561_HydraulicInlet2 > peakPsi2B){peakPsi2B = AI_HYD_psig_PT561_HydraulicInlet2;}
      }
      if(millis() - timer[3] > 3000 && timer[3]){
        deadHeadPsi2B = peakPsi2B;
        DO_HYD_XV557_DCV2_B = false;
        SUB_STATE2 = SIDE_A;
      }
    break;

    case SIDE_A:
      if(!timer[3]){ timer[3] = millis(); DO_HYD_XV554_DCV2_A = true; stateHistory2 = stateHistory2 + "+";}
      if(millis() - timer[3] > 500 && timer[3]){
        if(AI_HYD_psig_PT561_HydraulicInlet2 > deadHeadPsi2A-400){ DO_HYD_XV554_DCV2_A = false; }
      }
      if(millis() - timer[2] > switchingTime2A-550 && timer[2] && !DO_HYD_XV554_DCV2_A){//check if minimum time has passed
        SUB_STATE2 = SIDE_B;
        ///highCycleCnt++; //reached end of cycle time, switch sides 
      }
    break;

    case SIDE_B:
      if(!timer[3]){ timer[3] = millis(); DO_HYD_XV557_DCV2_B = true; stateHistory2 = stateHistory2 + "-";}
      if(millis() - timer[3] > 500 && timer[3]){
        if(AI_HYD_psig_PT561_HydraulicInlet2 > deadHeadPsi2B-400){ DO_HYD_XV557_DCV2_B = false; }
      }
      if(millis() - timer[2] > switchingTime2B-550 && timer[2] && !DO_HYD_XV557_DCV2_B){//check if minimum time has passed
        SUB_STATE2 = SIDE_A;
        //highCycleCnt++; //reached end of cycle time, switch sides 
      }
    break;

    case PAUSE:
      if(!timer[3]){ timer[3] = millis(); DO_HYD_XV554_DCV2_A = false; DO_HYD_XV557_DCV2_B = false;}
      
      if(manualPause){break;}
      for(int i = 0; i < PTsize;i++){
        if(PTdata[i].overPressure){ break; }
      }
      if(millis() - timer[3] > 5000 && timer[3]){ SUB_STATE2 = SIDE_A; }
      
    break;

    default:
    break;
  }
}
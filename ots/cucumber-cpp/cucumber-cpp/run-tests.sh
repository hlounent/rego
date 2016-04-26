#!/bin/sh

export BUILD_DIR=$1

./$BUILD_DIR/examples/FeatureShowcase/FeatureShowcaseSteps > /dev/null &
sleep 1; cucumber examples/FeatureShowcase
./$BUILD_DIR/examples/CalcQt/BoostCalculatorQtSteps > /dev/null &
sleep 1; cucumber examples/CalcQt
./$BUILD_DIR/examples/Calc/GTestCalculatorSteps > /dev/null &
sleep 1; cucumber examples/Calc
./$BUILD_DIR/examples/Calc/BoostCalculatorSteps > /dev/null &
sleep 1; cucumber examples/Calc

// taWrapper.cpp
//
// Matlab function:
// [varout] = taWrapper(taFunction,data,varin)
//
// 
// Inputs:
//		taFunction	The name of the TA-Lib function to call
//		data		A Mx4 array of prices in the form of Open | High | Low | Close
//		varin		The input variable(s) as necessary for the called taFunction
//
// Outputs:
//		varout		The output(s) as produced from the call to the taFunction

#include "mex.h"
#include "ta_libc.h"
#include <map>
#include <algorithm>	// So we can transform the function name string input ...
#include <string>		// from char to string ensuring lowercase

//#include "matrix.h" <-- May not be necessary

using namespace std;

// Value-Definitions of the different String values
static enum StringValue { taNotDefined, ta_acos, ta_ad, ta_add, ta_adosc, ta_adx, ta_adxr, ta_apo, ta_aroon, ta_aroonosc, ta_asine, ta_atan, ta_atr, ta_avgprice, ta_avgdev, ta_bbands, 
							ta_beta, ta_bop, ta_cci, 
							// Candlestick section start
							ta_cdl2crows, ta_cdl3blackcrows, ta_cdl3inside, ta_cdl3linestrike, ta_cdl3outside, ta_cdl3startsinsouth, ta_cdl3whitesoldiers, ta_cdlabandonedbaby, 
							ta_cdladvanceblock, ta_cdlbelthold, ta_cdlbreakaway, ta_cdlclosingmarubozu, ta_cdlconcealbabyswall, ta_cdlcounterattack, ta_cdldarkcloudcover, ta_cdldoji, 
							ta_cdldojistar, ta_cdldragonflydoji, ta_cdlengulfing, ta_cdleveningdojistar, ta_cdleveningstar, ta_cdlgapsidesidewhite, ta_cdlgravestonedoji, ta_cdlhammer, 
							ta_cdlhangingman, ta_cdlharami, ta_cdlharamicross, ta_cdlhighwave, ta_cdlhikkake, ta_cdlhikkakemod, ta_cdlhomingpigeon, ta_cdlidentical3crows, ta_cdlinneck, 
							ta_cdlinvertedhammer, ta_cdlkicking, ta_cdlkickingbylength, ta_cdlladderbottom, ta_cdllongleggeddoji, ta_cdllongline, ta_cdlmarubozu, ta_cdlmatchinglow, 
							ta_cdlmathold, ta_cdlmorningdojistar, ta_cdlmorningstar, ta_cdlonneck, ta_cdlpiercing, ta_cdlrickshawman, ta_cdlrisefall3methods, ta_cdlseparatinglines, 
							ta_cdlshootingstar, ta_cdlshortline, ta_cdlspinningtop, ta_cdlstalledpattern, ta_cdlsticksandwich, ta_cdltakuri, ta_cdltasukigap, ta_cdlthrusting, 
							ta_cdltristar, ta_cdlunique3river, ta_cdlupsidegap2crows, ta_cdlxsidegap3methods, 
							// Candlestick section end
							ta_ceil, ta_cmo, ta_correl, ta_cos, ta_cosh, ta_dema, ta_div, ta_dx, ta_ema, ta_exp, ta_floor, ta_ht_dcperiod, ta_ht_dcphase, ta_ht_phasor, ta_ht_sine, 
							ta_ht_trendline, ta_ht_trendmode, ta_kama, ta_linearreg, ta_linearreg_angle, ta_linearreg_intercept, ta_linearreg_slope, ta_ln, ta_log10, ta_ma, ta_macd, 
							ta_macdext, ta_macdfix, ta_mama, ta_mavp, ta_max, ta_maxindex, ta_medprice, ta_mfi, ta_midpoint, ta_midprice, ta_min, ta_minindex, ta_minmax, ta_minmaxindex, 
							ta_minus_di, ta_minus_dm, ta_mom, ta_mult, ta_natr, ta_obv, ta_plus_di, ta_plus_dm, ta_ppo, ta_roc, ta_rocp, ta_rocr, ta_rocr100, ta_rsi, ta_sar, ta_sarext,
							ta_sin, ta_sinh, ta_sma, ta_sqrt, ta_stddev, ta_stoch, ta_stochf, ta_stochrsi, ta_sub, ta_sum, ta_t3, ta_tan, ta_tanh, ta_tema, ta_trange, ta_trima, ta_trix, 
							ta_tsf, ta_typprice, ta_ultosc, ta_var, ta_wclprice, ta_willr, ta_wma};

// Map to associate the strings with the enum values
static map<string, StringValue> s_mapStringValues;

// Initialization
static void InitSwitchMapping();

void mexFunction(int nlhs, mxArray *plhs[], /* Output variables */
				 int nrhs, const mxArray *prhs[]) /* Input variables */
{
	// Check number of inputs
	if (nrhs < 2)
		mexErrMsgIdAndTxt( "MATLAB:taWrapper:NumInputs",
		"Number of input arguments is not correct. Aborting (26).");

	// Define constants (#define assigns a variable as either a constant or a macro)
	// Inputs
#define taFuncName_IN		prhs[0]

	/* Assign pointers to the function string */ 
	double * funcNamePtr = mxGetPr(taFuncName_IN);
	int funcNumChars = mxGetN(taFuncName_IN)+1;	// +1 for the NULL added at the end

	char *funcAsChars;

	//http://www.math.ufl.edu/help/matlab/mxCreateString.html
	mxGetString(taFuncName_IN, funcAsChars, funcNumChars);

	string taFuncName((funcAsChars));
	transform(taFuncName.begin(), taFuncName.end(), taFuncName.begin(), ::tolower);

	// Init the string mapping
	InitSwitchMapping();

	switch (s_mapStringValues[taFuncName])
	{
		// Vector Trigonometric ACos
		case ta_acos:

			break;
	
		// Chaikin A/D Line
		// ad = taWrapper('ta_ad', high, low, close, vol);
		case ta_ad:       
			// Check number of inputs
			if (nrhs != 5)
				mexErrMsgIdAndTxt( "MATLAB:taWrapper:ta_ad:NumInputs",
				"Number of input arguments is not correct. Price data should be parsed into vectors H | L | C followed by a volume vector V. Aborting (232).");
			if (nlhs != 1)
				mexErrMsgIdAndTxt( "MATLAB:taWrapper:ta_ad:NumOutputs",
					"The function 'ta_ad' (Chaikin A/D Line) produces a single vector output. Aborting (235).");
			#define high_IN		prhs[1]
			#define low_IN		prhs[2]
			#define close_IN	prhs[3]
			#define vol_IN		prhs[4]

			// Declare variables
			int startIdx, endIdx, rows, colsH, colsL, colsC, colsV;
			double *highPtr, *lowPtr, *closePtr, *volPtr;

			// Initialize error handling 
			TA_RetCode retCode;

			// Parse inputs and error check
			// Assign pointers and get dimensions
			highPtr		= mxGetPr(high_IN);
			rows		= mxGetM(high_IN);
			colsH		= mxGetN(high_IN);
			lowPtr		= mxGetPr(low_IN);
			colsL		= mxGetN(low_IN);
			closePtr	= mxGetPr(close_IN);
			colsC		= mxGetN(close_IN);
			volPtr		= mxGetPr(vol_IN);
			colsV		= mxGetN(vol_IN);

			// Input validation
			if (colsH != 1)
			{
				mexErrMsgIdAndTxt( "MATLAB:taWrapper:ta_ad:InputErr",
					"Price data should be passed to the function already parsed into H | L | C vectors.\nThe 'High' vector had more than 1 column.  Aborting (266).");
			}

			if (colsL != 1)
			{
				mexErrMsgIdAndTxt( "MATLAB:taWrapper:ta_ad:InputErr",
					"Price data should be passed to the function already parsed into H | L | C vectors.\nThe 'Low' vector had more than 1 column.  Aborting (271).");
			}

			if (colsC != 1)
			{
				mexErrMsgIdAndTxt( "MATLAB:taWrapper:ta_ad:InputErr",
					"Price data should be passed to the function already parsed into H | L | C vectors.\nThe 'Close' vector had more than 1 column.  Aborting (277).");
			}

			if (colsV != 1)
			{
				mexErrMsgIdAndTxt( "MATLAB:taWrapper:ta_ad:InputErr",
					"Volume data should be a single vector array. Aborting (283).");
			}

			endIdx = rows - 1;  // Adjust for C++ starting at '0'
			startIdx = 0;

			// Output variables
			int adIdx, outElements;
			double *outReal;

			// Preallocate heap
			outReal = (double*)mxCalloc(rows, sizeof(double));			// added cast

			// Invoke
			retCode = TA_AD( startIdx, endIdx, highPtr, lowPtr, closePtr, volPtr, &adIdx, &outElements, outReal);
		
			// Error handling
			if (retCode) 
			{
				mxFree(outReal);
				mexPrintf("%s%i","Return code=",retCode);
				mexErrMsgTxt("Invocation to 'ta_ad' failed. Aborting (305).");
			}

			// Populate Output
			plhs[0] = mxCreateDoubleMatrix(adIdx+outElements,1, mxREAL);
			memcpy(((double*) mxGetData(plhs[0]))+adIdx, outReal, outElements*mxGetElementSize(plhs[0]));
		
			// Cleanup
			mxFree(outReal);  
			break;

		// Vector Arithmetic Add
		case ta_add:       

			break;
		// Chaikin A/D Oscillator
		case ta_adosc:       

			break;
		// Average Directional Movement Index
		case ta_adx:       

			break;
		// Average Directional Movement Index Rating
		case ta_adxr:       

			break;
		// Absolute Price Oscillator
		case ta_apo:       

			break;
		// Aroon
		case ta_aroon:       

			break;
		// Aroon Oscillator
		case ta_aroonosc:       

			break;
		// Vector Trigonometric ASin
		case ta_asine:       

			break;
		// Vector Trigonometric ATan
		case ta_atan:       

			break;
		// Average True Range
		case ta_atr:       

			break;
		// Average Price
		case ta_avgprice:       

			break;
		// Average Deviation
		case ta_avgdev:       

			break;
		// Bollinger Bands
		case ta_bbands:       

			break;
		// Beta
		case ta_beta:       

			break;
		// Balance Of Power
		case ta_bop:       

			break;
		// Commodity Channel Index
		case ta_cci:       

			break;
		// Two Crows
		case ta_cdl2crows:       

			break;
		// Three Black Crows
		case ta_cdl3blackcrows:       

			break;
		// Three Inside Up/Down
		case ta_cdl3inside:       

			break;
		// Three-Line Strike
		case ta_cdl3linestrike:       

			break;
		// Three Outside Up/Down
		case ta_cdl3outside:       

			break;
		// Three Stars In The South
		case ta_cdl3startsinsouth:       

			break;
		// Three Advancing White Soldiers
		case ta_cdl3whitesoldiers:       

			break;
		// Abandoned Baby
		case ta_cdlabandonedbaby:       

			break;
		// Advance Block
		case ta_cdladvanceblock:       

			break;
		// Belt-hold
		case ta_cdlbelthold:       

			break;
		// Breakaway
		case ta_cdlbreakaway:       

			break;
		// Closing Marubozu
		case ta_cdlclosingmarubozu:       

			break;
		// Concealing Baby Swallow
		case ta_cdlconcealbabyswall:       

			break;
		// Counterattack
		case ta_cdlcounterattack:       

			break;
		// Dark Cloud Cover
		case ta_cdldarkcloudcover:       

			break;
		// Doji
		case ta_cdldoji:       

			break;
		// Doji Star
		case ta_cdldojistar:       

			break;
		// Dragonfly Doji
		case ta_cdldragonflydoji:       

			break;
		// Engulfing Pattern
		case ta_cdlengulfing:       

			break;
		// Evening Doji Star
		case ta_cdleveningdojistar:       

			break;
		// Evening Star
		case ta_cdleveningstar:       

			break;
		// Up/Down-gap side-by-side white lines
		case ta_cdlgapsidesidewhite:       

			break;
		// Gravestone Doji
		case ta_cdlgravestonedoji:       

			break;
		// Hammer
		case ta_cdlhammer:       

			break;
		// Hanging Man
		case ta_cdlhangingman:       

			break;
		// Harami Pattern
		case ta_cdlharami:       

			break;
		// Harami Cross Pattern
		case ta_cdlharamicross:       

			break;
		// High-Wave Candle
		case ta_cdlhighwave:       

			break;
		// Hikkake Pattern
		case ta_cdlhikkake:       

			break;
		// Modified Hikkake Pattern
		case ta_cdlhikkakemod:       

			break;
		// Homing Pigeon
		case ta_cdlhomingpigeon:       

			break;
		// Identical Three Crows
		case ta_cdlidentical3crows:       

			break;
		// In-Neck Pattern
		case ta_cdlinneck:       

			break;
		// Inverted Hammer
		case ta_cdlinvertedhammer:       

			break;
		// Kicking
		case ta_cdlkicking:       

			break;
		// Kicking - bull/bear determined by the longer Marubozu
		case ta_cdlkickingbylength:       

			break;
		// Ladder Bottom
		case ta_cdlladderbottom:       

			break;
		// Long Legged Doji
		case ta_cdllongleggeddoji:       

			break;
		// Long Line Candle
		case ta_cdllongline:       

			break;
		// Marubozu
		case ta_cdlmarubozu:       

			break;
		// Matching Low
		case ta_cdlmatchinglow:       

			break;
		// Mat Hold
		case ta_cdlmathold:       

			break;
		// Morning Doji Star
		case ta_cdlmorningdojistar:       

			break;
		// Morning Star
		case ta_cdlmorningstar:       

			break;
		// On-Neck Pattern
		case ta_cdlonneck:       

			break;
		// Piercing Pattern
		case ta_cdlpiercing:       

			break;
		// Rickshaw Man
		case ta_cdlrickshawman:       

			break;
		// Rising/Falling Three Methods
		case ta_cdlrisefall3methods:       

			break;
		// Separating Lines
		case ta_cdlseparatinglines:       

			break;
		// Shooting Star
		case ta_cdlshootingstar:       

			break;
		// Short Line Candle
		case ta_cdlshortline:       

			break;
		// Spinning Top
		case ta_cdlspinningtop:       

			break;
		// Stalled Pattern
		case ta_cdlstalledpattern:       

			break;
		// Stick Sandwich
		case ta_cdlsticksandwich:       

			break;
		// Takuri (Dragonfly Doji with very long lower shadow)
		case ta_cdltakuri:       

			break;
		// Tasuki Gap
		case ta_cdltasukigap:       

			break;
		// Thrusting Pattern
		case ta_cdlthrusting:       

			break;
		// Tristar Pattern
		case ta_cdltristar:       

			break;
		// Unique 3 River
		case ta_cdlunique3river:       

			break;
		// Upside Gap Two Crows
		case ta_cdlupsidegap2crows:       

			break;
		// Upside/Downside Gap Three Methods
		case ta_cdlxsidegap3methods:       

			break;
		// Vector Ceil
		case ta_ceil:       

			break;
		// Chande Momentum Oscillator
		case ta_cmo:       

			break;
		// Pearson's Correlation Coefficient (r)
		case ta_correl:       

			break;
		// Vector Trigonometric Cos
		case ta_cos:       

			break;
		// Vector Trigonometric Cosh
		case ta_cosh:       

			break;
		// Double Exponential Moving Average
		case ta_dema:       

			break;
		// Vector Arithmetic Div
		case ta_div:       

			break;
		// Directional Movement Index
		case ta_dx:       

			break;
		// Exponential Moving Average
		case ta_ema:       

			break;
		// Vector Arithmetic Exp
		case ta_exp:       

			break;
		// Vector Floor
		case ta_floor:       

			break;
		// Hilbert Transform - Dominant Cycle Period
		case ta_ht_dcperiod:       

			break;
		// Hilbert Transform - Dominant Cycle Phase
		case ta_ht_dcphase:       

			break;
		// Hilbert Transform - Phasor Components
		case ta_ht_phasor:       

			break;
		// Hilbert Transform - SineWave
		case ta_ht_sine:       

			break;
		// Hilbert Transform - Instantaneous Trendline
		case ta_ht_trendline:       

			break;
		// Hilbert Transform - Trend vs Cycle Mode
		case ta_ht_trendmode:       

			break;
		// Kaufman Adaptive Moving Average
		case ta_kama:       

			break;
		// Linear Regression
		case ta_linearreg:       

			break;
		// Linear Regression Angle
		case ta_linearreg_angle:       

			break;
		// Linear Regression Intercept
		case ta_linearreg_intercept:       

			break;
		// Linear Regression Slope
		case ta_linearreg_slope:       

			break;
		// Vector Log Natural
		case ta_ln:       

			break;
		// Vector Log10
		case ta_log10:       

			break;
		// Moving average
		case ta_ma:       

			break;
		// Moving Average Convergence/Divergence
		case ta_macd:       

			break;
		// MACD with controllable MA type
		case ta_macdext:       

			break;
		// Moving Average Convergence/Divergence Fix 12/26
		case ta_macdfix:       

			break;
		// MESA Adaptive Moving Average
		case ta_mama:       

			break;
		// Moving average with variable period
		case ta_mavp:       

			break;
		// Highest value over a specified period
		case ta_max:       

			break;
		// Index of highest value over a specified period
		case ta_maxindex:       

			break;
		// Median Price
		case ta_medprice:       

			break;
		// Money Flow Index
		case ta_mfi:       

			break;
		// MidPoint over period
		case ta_midpoint:       

			break;
		// Midpoint Price over period
		case ta_midprice:       

			break;
		// Lowest value over a specified period
		case ta_min:       

			break;
		// Index of lowest value over a specified period
		case ta_minindex:       

			break;
		// Lowest and highest values over a specified period
		case ta_minmax:       

			break;
		// Indexes of lowest and highest values over a specified period
		case ta_minmaxindex:       

			break;
		// Minus Directional Indicator
		case ta_minus_di:       

			break;
		// Minus Directional Movement
		case ta_minus_dm:       

			break;
		// Momentum
		case ta_mom:       

			break;
		// Vector Arithmetic Mult
		case ta_mult:       

			break;
		// Normalized Average True Range
		case ta_natr:       

			break;
		// On Balance Volume
		case ta_obv:       

			break;
		// Plus Directional Indicator
		case ta_plus_di:       

			break;
		// Plus Directional Movement
		case ta_plus_dm:       

			break;
		// Percentage Price Oscillator
		case ta_ppo:       

			break;
		// Rate of change : ((price/prevPrice)-1)*100
		case ta_roc:       

			break;
		// Rate of change Percentage: (price-prevPrice)/prevPrice
		case ta_rocp:       

			break;
		// Rate of change ratio: (price/prevPrice)
		case ta_rocr:       

			break;
		// Rate of change ratio 100 scale: (price/prevPrice)*100
		case ta_rocr100:       

			break;
		// Relative Strength Index
		case ta_rsi:       

			break;
		// Parabolic SAR
		case ta_sar:       

			break;
		// Parabolic SAR - Extended
		case ta_sarext:       

			break;
		// Vector Trigonometric Sin
		case ta_sin:       

			break;
		// Vector Trigonometric Sinh
		case ta_sinh:       

			break;
		// Simple Moving Average
		case ta_sma:       

			break;
		// Vector Square Root
		case ta_sqrt:       

			break;
		// Standard Deviation
		case ta_stddev:       

			break;
		// Stochastic
		case ta_stoch:       

			break;
		// Stochastic Fast
		case ta_stochf:       

			break;
		// Stochastic Relative Strength Index
		case ta_stochrsi:       

			break;
		// Vector Arithmetic Subtraction
		case ta_sub:       

			break;
		// Summation
		case ta_sum:       

			break;
		// Triple Exponential Moving Average (T3)
		case ta_t3:       

			break;
		// Vector Trigonometric Tan
		case ta_tan:       

			break;
		// Vector Trigonometric Tanh
		case ta_tanh:       
	
			break;
		// Triple Exponential Moving Average
		case ta_tema:       

			break;
		// True Range
		case ta_trange:       

			break;
		// Triangular Moving Average
		case ta_trima:       

			break;
		// 1-day Rate-Of-Change (ROC) of a Triple Smooth EMA
		case ta_trix:       

			break;
		// Time Series Forecast
		case ta_tsf:       

			break;
		// Typical Price
		case ta_typprice:       

			break;
		// Ultimate Oscillator
		case ta_ultosc:       

			break;
		// Variance
		case ta_var:       

			break;
		// Weighted Close Price
		case ta_wclprice:       

			break;
		// Williams' %R
		case ta_willr:       

			break;
		// Weighted Moving Average
		case ta_wma:       

			break;

		// Unknown function given as input
		default:
			mexErrMsgIdAndTxt( "MATLAB:taWrapper:UnknownFunction",
				"Unable to find a matching function to: %s. Aborting (864).",taFuncName);
			break;
	}

}

void InitSwitchMapping()
{
	s_mapStringValues["ta_acos"]				= ta_acos;
	s_mapStringValues["ta_ad"]					= ta_ad;
	s_mapStringValues["ta_add"]					= ta_add;
	s_mapStringValues["ta_adosc"]				= ta_adosc;
	s_mapStringValues["ta_adx"]					= ta_adx;
	s_mapStringValues["ta_adxr"]				= ta_adxr;
	s_mapStringValues["ta_apo"]					= ta_apo;
	s_mapStringValues["ta_aroon"]				= ta_aroon;
	s_mapStringValues["ta_aroonosc"]			= ta_aroonosc;
	s_mapStringValues["ta_asine"]				= ta_asine;
	s_mapStringValues["ta_atan"]				= ta_atan;
	s_mapStringValues["ta_atr"]					= ta_atr;
	s_mapStringValues["ta_avgprice"]			= ta_avgprice;
	s_mapStringValues["ta_avgdev"]				= ta_avgdev;
	s_mapStringValues["ta_bbands"]				= ta_bbands;
	s_mapStringValues["ta_beta"]				= ta_beta;
	s_mapStringValues["ta_bop"]					= ta_bop;
	s_mapStringValues["ta_cci"]					= ta_cci;
	s_mapStringValues["ta_cdl2crows"]			= ta_cdl2crows;
	s_mapStringValues["ta_cdl3blackcrows"]		= ta_cdl3blackcrows;
	s_mapStringValues["ta_cdl3inside"]			= ta_cdl3inside;
	s_mapStringValues["ta_cdl3linestrike"]		= ta_cdl3linestrike;
	s_mapStringValues["ta_cdl3outside"]			= ta_cdl3outside;
	s_mapStringValues["ta_cdl3startsinsouth"]	= ta_cdl3startsinsouth;
	s_mapStringValues["ta_cdl3whitesoldiers"]	= ta_cdl3whitesoldiers;
	s_mapStringValues["ta_cdlabandonedbaby"]	= ta_cdlabandonedbaby;
	s_mapStringValues["ta_cdladvanceblock"]		= ta_cdladvanceblock;
	s_mapStringValues["ta_cdlbelthold"]			= ta_cdlbelthold;
	s_mapStringValues["ta_cdlbreakaway"]		= ta_cdlbreakaway;
	s_mapStringValues["ta_cdlclosingmarubozu"]	= ta_cdlclosingmarubozu;
	s_mapStringValues["ta_cdlconcealbabyswall"]	= ta_cdlconcealbabyswall;
	s_mapStringValues["ta_cdlcounterattack"]	= ta_cdlcounterattack;
	s_mapStringValues["ta_cdldarkcloudcover"]	= ta_cdldarkcloudcover;
	s_mapStringValues["ta_cdldoji"]				= ta_cdldoji;
	s_mapStringValues["ta_cdldojistar"]			= ta_cdldojistar;
	s_mapStringValues["ta_cdldragonflydoji"]	= ta_cdldragonflydoji;
	s_mapStringValues["ta_cdlengulfing"]		= ta_cdlengulfing;
	s_mapStringValues["ta_cdleveningdojistar"]	= ta_cdleveningdojistar;
	s_mapStringValues["ta_cdleveningstar"]		= ta_cdleveningstar;
	s_mapStringValues["ta_cdlgapsidesidewhite"]	= ta_cdlgapsidesidewhite;
	s_mapStringValues["ta_cdlgravestonedoji"]	= ta_cdlgravestonedoji;
	s_mapStringValues["ta_cdlhammer"]			= ta_cdlhammer;
	s_mapStringValues["ta_cdlhangingman"]		= ta_cdlhangingman;
	s_mapStringValues["ta_cdlharami"]			= ta_cdlharami;
	s_mapStringValues["ta_cdlharamicross"]		= ta_cdlharamicross;
	s_mapStringValues["ta_cdlhighwave"]			= ta_cdlhighwave;
	s_mapStringValues["ta_cdlhikkake"]			= ta_cdlhikkake;
	s_mapStringValues["ta_cdlhikkakemod"]		= ta_cdlhikkakemod;
	s_mapStringValues["ta_cdlhomingpigeon"]		= ta_cdlhomingpigeon;
	s_mapStringValues["ta_cdlidentical3crows"]	= ta_cdlidentical3crows;
	s_mapStringValues["ta_cdlinneck"]			= ta_cdlinneck;
	s_mapStringValues["ta_cdlinvertedhammer"]	= ta_cdlinvertedhammer;
	s_mapStringValues["ta_cdlkicking"]			= ta_cdlkicking;
	s_mapStringValues["ta_cdlkickingbylength"]	= ta_cdlkickingbylength;
	s_mapStringValues["ta_cdlladderbottom"]		= ta_cdlladderbottom;
	s_mapStringValues["ta_cdllongleggeddoji"]	= ta_cdllongleggeddoji;
	s_mapStringValues["ta_cdllongline"]			= ta_cdllongline;
	s_mapStringValues["ta_cdlmarubozu"]			= ta_cdlmarubozu;
	s_mapStringValues["ta_cdlmatchinglow"]		= ta_cdlmatchinglow;
	s_mapStringValues["ta_cdlmathold"]			= ta_cdlmathold;
	s_mapStringValues["ta_cdlmorningdojistar"]	= ta_cdlmorningdojistar;
	s_mapStringValues["ta_cdlmorningstar"]		= ta_cdlmorningstar;
	s_mapStringValues["ta_cdlonneck"]			= ta_cdlonneck;
	s_mapStringValues["ta_cdlpiercing"]			= ta_cdlpiercing;
	s_mapStringValues["ta_cdlrickshawman"]		= ta_cdlrickshawman;
	s_mapStringValues["ta_cdlrisefall3methods"]	= ta_cdlrisefall3methods;
	s_mapStringValues["ta_cdlseparatinglines"]	= ta_cdlseparatinglines;
	s_mapStringValues["ta_cdlshootingstar"]		= ta_cdlshootingstar;
	s_mapStringValues["ta_cdlshortline"]		= ta_cdlshortline;
	s_mapStringValues["ta_cdlspinningtop"]		= ta_cdlspinningtop;
	s_mapStringValues["ta_cdlstalledpattern"]	= ta_cdlstalledpattern;
	s_mapStringValues["ta_cdlsticksandwich"]	= ta_cdlsticksandwich;
	s_mapStringValues["ta_cdltakuri"]			= ta_cdltakuri;
	s_mapStringValues["ta_cdltasukigap"]		= ta_cdltasukigap;
	s_mapStringValues["ta_cdlthrusting"]		= ta_cdlthrusting;
	s_mapStringValues["ta_cdltristar"]			= ta_cdltristar;
	s_mapStringValues["ta_cdlunique3river"]		= ta_cdlunique3river;
	s_mapStringValues["ta_cdlupsidegap2crows"]	= ta_cdlupsidegap2crows;
	s_mapStringValues["ta_cdlxsidegap3methods"]	= ta_cdlxsidegap3methods;
	s_mapStringValues["ta_ceil"]				= ta_ceil;
	s_mapStringValues["ta_cmo"]					= ta_cmo;
	s_mapStringValues["ta_correl"]				= ta_correl;
	s_mapStringValues["ta_cos"]					= ta_cos;
	s_mapStringValues["ta_cosh"]				= ta_cosh;
	s_mapStringValues["ta_dema"]				= ta_dema;
	s_mapStringValues["ta_div"]					= ta_div;
	s_mapStringValues["ta_dx"]					= ta_dx;
	s_mapStringValues["ta_ema"]					= ta_ema;
	s_mapStringValues["ta_exp"]					= ta_exp;
	s_mapStringValues["ta_floor"]				= ta_floor;
	s_mapStringValues["ta_ht_dcperiod"]			= ta_ht_dcperiod;
	s_mapStringValues["ta_ht_dcphase"]			= ta_ht_dcphase;
	s_mapStringValues["ta_ht_phasor"]			= ta_ht_phasor;
	s_mapStringValues["ta_ht_sine"]				= ta_ht_sine;
	s_mapStringValues["ta_ht_trendline"]		= ta_ht_trendline;
	s_mapStringValues["ta_ht_trendmode"]		= ta_ht_trendmode;
	s_mapStringValues["ta_kama"]				= ta_kama;
	s_mapStringValues["ta_linearreg"]			= ta_linearreg;
	s_mapStringValues["ta_linearreg_angle"]		= ta_linearreg_angle;
	s_mapStringValues["ta_linearreg_intercept"]	= ta_linearreg_intercept;
	s_mapStringValues["ta_linearreg_slope"]		= ta_linearreg_slope;
	s_mapStringValues["ta_ln"]					= ta_ln;
	s_mapStringValues["ta_log10"]				= ta_log10;
	s_mapStringValues["ta_ma"]					= ta_ma;
	s_mapStringValues["ta_macd"]				= ta_macd;
	s_mapStringValues["ta_macdext"]				= ta_macdext;
	s_mapStringValues["ta_macdfix"]				= ta_macdfix;
	s_mapStringValues["ta_mama"]				= ta_mama;
	s_mapStringValues["ta_mavp"]				= ta_mavp;
	s_mapStringValues["ta_max"]					= ta_max;
	s_mapStringValues["ta_maxindex"]			= ta_maxindex;
	s_mapStringValues["ta_medprice"]			= ta_medprice;
	s_mapStringValues["ta_mfi"]					= ta_mfi;
	s_mapStringValues["ta_midpoint"]			= ta_midpoint;
	s_mapStringValues["ta_midprice"]			= ta_midprice;
	s_mapStringValues["ta_min"]					= ta_min;
	s_mapStringValues["ta_minindex"]			= ta_minindex;
	s_mapStringValues["ta_minmax"]				= ta_minmax;
	s_mapStringValues["ta_minmaxindex"]			= ta_minmaxindex;
	s_mapStringValues["ta_minus_di"]			= ta_minus_di;
	s_mapStringValues["ta_minus_dm"]			= ta_minus_dm;
	s_mapStringValues["ta_mom"]					= ta_mom;
	s_mapStringValues["ta_mult"]					= ta_mult;
	s_mapStringValues["ta_natr"]				= ta_natr;
	s_mapStringValues["ta_obv"]					= ta_obv;
	s_mapStringValues["ta_plus_di"]				= ta_plus_di;
	s_mapStringValues["ta_plus_dm"]				= ta_plus_dm;
	s_mapStringValues["ta_ppo"]					= ta_ppo;
	s_mapStringValues["ta_roc"]					= ta_roc;
	s_mapStringValues["ta_rocp"]				= ta_rocp;
	s_mapStringValues["ta_rocr"]				= ta_rocr;
	s_mapStringValues["ta_rocr100"]				= ta_rocr100;
	s_mapStringValues["ta_rsi"]					= ta_rsi;
	s_mapStringValues["ta_sar"]					= ta_sar;
	s_mapStringValues["ta_sarext"]				= ta_sarext;
	s_mapStringValues["ta_sin"]					= ta_sin;
	s_mapStringValues["ta_sinh"]				= ta_sinh;
	s_mapStringValues["ta_sma"]					= ta_sma;
	s_mapStringValues["ta_sqrt"]				= ta_sqrt;
	s_mapStringValues["ta_stddev"]				= ta_stddev;
	s_mapStringValues["ta_stoch"]				= ta_stoch;
	s_mapStringValues["ta_stochf"]				= ta_stochf;
	s_mapStringValues["ta_stochrsi"]			= ta_stochrsi;
	s_mapStringValues["ta_sub"]					= ta_sub;
	s_mapStringValues["ta_sum"]					= ta_sum;
	s_mapStringValues["ta_t3"]					= ta_t3;
	s_mapStringValues["ta_tan"]					= ta_tan;
	s_mapStringValues["ta_tanh"]				= ta_tanh;
	s_mapStringValues["ta_tema"]				= ta_tema;
	s_mapStringValues["ta_trange"]				= ta_trange;
	s_mapStringValues["ta_trima"]				= ta_trima;
	s_mapStringValues["ta_trix"]				= ta_trix;
	s_mapStringValues["ta_tsf"]					= ta_tsf;
	s_mapStringValues["ta_typprice"]			= ta_typprice;
	s_mapStringValues["ta_ultosc"]				= ta_ultosc;
	s_mapStringValues["ta_var"]					= ta_var;
	s_mapStringValues["ta_wclprice"]			= ta_wclprice;
	s_mapStringValues["ta_willr"]				= ta_willr;
	s_mapStringValues["ta_wma"]					= ta_wma;

}


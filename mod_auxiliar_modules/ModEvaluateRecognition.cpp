#include "headers.h"
#include "SSFCore.h"
#include "LAP.h"
#include "ModEvaluateRecognition.h"

ModEvaluateRecognition instance("");

/******
 * class EvaluateRecognition
 ******/
ModEvaluateRecognition::ModEvaluateRecognition(string modID) :  SSFUserModule(modID) {

	outFile = "results.csv";
	outType = "CMC";
	maxRank = -1;

	/* Add parameters */
	inputParams.Add("outFile", &outFile, "File ", false);
	inputParams.Add("outType", &outType, "File ", false);
	inputParams.Add("maxRank", &maxRank, "File ", false);

	/* module information */
	info.Description("Module to evaluate recognition performance.");

	/* set input and output data */
	modParams.Require(modID, SSF_DATA_OBJECT, &inModObjPred, "predict");		// input objects
	modParams.Require(modID, SSF_DATA_OBJECT, &inModObjLabels, "labels");		// Ground truth input objects
	modParams.Require(modID, SSF_DATA_OBJECTCANDIDATES, &inModObjcCandidates);	// input object candidates
	
	/* register the module */
	this->Register(this, modID);
}

SSFMethod *ModEvaluateRecognition::Instantiate(string modID) {

	return new ModEvaluateRecognition(modID); 
}


void ModEvaluateRecognition::Setup() {

	if( inModObjLabels == "" )
		ReportError("Module with the correct object labels not set (attribute: labels)!");

	if( outType != "CMC" && outType != "CM" )
		ReportError("Output type can be only CMC or CM (variable: outFile)!");

	if( outType == "CM" && inModObjPred == "" )
		ReportError("Object input module not set (attribute: predict)!");

	if( outType == "CMC" && inModObjcCandidates == "" )
		ReportError("Object candidates input module not set!");
}


void ModEvaluateRecognition::CM() {
int i;
SMIndexType position = 0;
SSFObject *object;
vector<SMIndexType> trackletIDs;
string objID;
string objIDPred;
unordered_map<SMIndexType, SMIndexType>::iterator it;
unordered_map< string, unordered_map< string, int > > confMap;
unordered_set<string> headers;
unordered_set<string>::iterator iheaders;
unordered_map< string, unordered_map< string, int > >::iterator ilabel;

	while( (object = glb_sharedMem.RetrieveObject(inModObjPred, position++)) != NULL) {

		trackletIDs = object->GetTrackletIDs();

		objIDPred = object->GetObjID();

		headers.insert(objIDPred);

		for( i = 0; i < trackletIDs.size(); ++i ) {

			// get object ID
			it = trackletID2ObjID.find( trackletIDs[ i ] );
			if( it == trackletID2ObjID.end() )
				ReportError("Can't find object label for tracklet %d.", trackletIDs[ i ]);
			objID = glb_sharedMem.RetrieveObject( it->second )->GetObjID();

			// update matrix
			if( confMap[ objID ].find( objIDPred ) == confMap[ objID ].end() )
				confMap[ objID ][ objIDPred ] = 1;
			else
				confMap[ objID ][ objIDPred ]++;
		}
	}

	// print headers
	iheaders = headers.begin();
	out << *iheaders;
	for( ++iheaders; iheaders != headers.end(); ++iheaders )
		out << "," << *iheaders;
	out << "\n";
	// print matrix
	for( ilabel = confMap.begin(); ilabel != confMap.end(); ++ilabel ) {
		out << ilabel->first;
		for( iheaders = headers.begin(); iheaders != headers.end(); ++iheaders ) {
			if( ilabel->second.find( *iheaders ) == ilabel->second.end() )
				out << "," << 0;
			else
				out << "," << ilabel->second[ *iheaders ];
		}
		out << "\n";
	}
}

void ModEvaluateRecognition::CMC() {
int i;
string objID;
SMIndexType position = 0;
SSFObjectCandidates *objCandidates;
const vector<SSFObjectCandidate> *candidates;
bool found;
string objIDPred;
unordered_map<string, int> counts;
unordered_map<string, vector<float> > ranks;

SSFMatrix<float> rank;
unordered_map<SMIndexType, SMIndexType>::iterator it;
unordered_map<string, vector<float> >::iterator irk;

	while( (objCandidates = glb_sharedMem.GetObjectCandidates(inModObjcCandidates, position++)) != NULL ) {

		// get object candidates
		candidates = objCandidates->GetObjectCandidates();
		if( candidates->empty() )
			ReportError("There is no object candidate for tracklet %d of module %s.", objCandidates->GetTrackletID(), inModObjcCandidates.c_str());

		// set maxRank
		if( maxRank < 0 )
			maxRank = candidates->size();

		// get object ID
		it = trackletID2ObjID.find( objCandidates->GetTrackletID() );
		if( it == trackletID2ObjID.end() )
			ReportError("Can't find object label for tracklet %d.", objCandidates->GetTrackletID());
		objID = glb_sharedMem.RetrieveObject( it->second )->GetObjID();

		// create a new rank counting vector
		if( ranks[ objID ].empty() ) {
			ranks[ objID ].assign( maxRank, 0 );
			counts[ objID ] = 0;
		}

		// update ranks
		found = false;
		for( i = 0; i < maxRank; ++i ) {

			if( i >= candidates->size() )
				ReportError("Module %s generated less candidates (%d) for tracklet %d than the rank informed (%d)", inModObjcCandidates.c_str(), i, objCandidates->GetTrackletID(), maxRank);

			if( !found ) {
				objIDPred = glb_sharedMem.RetrieveObject( (*candidates)[i].objCandidate )->GetObjID();
				if( objID == objIDPred ) {
					found = true;
					ranks[ objID ][i]++;
				}
			}
			else
				ranks[ objID ][i]++;
 		}

		// update counts for that object
		counts[ objID ]++;
	}

	rank = SSFMatrix<float>::zeros( maxRank, 2 );
	for( irk = ranks.begin(); irk != ranks.end(); ++irk ) {
		for( i = 0; i < irk->second.size(); ++i ) {
			rank.at<float>( i, 1 ) += irk->second[i] / counts[ irk->first ];
		}
	}
	for( i = 0; i < rank.rows; ++i ) {
		rank.at<float>( i, 0 ) =  i+1;
		rank.at<float>( i, 1 ) /= ranks.size();
	}
	//out << "rank,rrate\n";
	out << format(rank, "csv");
}

void ModEvaluateRecognition::Execute() {
int i;
SMIndexType position = 0;
SSFObject *object;
vector<SMIndexType> tracklets;

	// associate trackletID to objectID
	while( (object = glb_sharedMem.RetrieveObject(inModObjLabels, position++)) != NULL ) {

		// get tracklets
		tracklets = object->GetTrackletIDs();

		// associate tracklets with the objectID
		for( i = 0; i < tracklets.size(); ++i )
			trackletID2ObjID[ tracklets[i] ] = object->GetIndex();
	}

	// evaluate
	out.open( outFile.c_str(), ios::out );
	if( !out.is_open() )
		ReportError("Couldn't open file %s for write.", outFile.c_str());
	if( outType == "CMC" ) {
		CMC();
	}
	else if( outType == "CM" ) {
		CM();
	}
	else {
		out.close();
		ReportError("Output type can be only CMC or CM (variable: outType)");
	}
	out.close();
}

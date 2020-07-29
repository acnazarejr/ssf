#include "headers.h"
#include "LAP.h"
#include "timeProfiler.h"




TimeProfileThread::TimeProfileThread() {

	this->id = 0;
}




void TimeProfileThread::Start2(const char *file, const char *function, int line) {
map<string,int>::iterator it;
TimeProfileEntry *currEntry;
string str;
int entryId;
int64 t1;

	t1 = cvGetTickCount();

	str = "[" + glb_SSFExecCtrl.GetInstanceIDCurrentThread() + "] " + string(function) + "():" + IntToString(line);

	mutex.Lock();
	if (executionStackIds.size() > 0)
		str = str + "_" +  IntToString(executionStackIds.back());

	executionStack.push_back(str);
	

	it = entryMap.find(str);
	if (it == entryMap.end()) {
		entryMap.insert(pair<string,int>(str, id++));
		entryId = id-1;
		currEntry = new TimeProfileEntry();
		currEntry->count = 0;
		currEntry->threadID = glb_SSFExecCtrl.GetInstanceIDCurrentThread();
		currEntry->line0 = line;
		currEntry->line1 = -1;
		currEntry->name = function;
		currEntry->ntimes = 0;
		currEntry->owntime = 0;
		currEntry->total = 0;
		currEntry->filename = file;
		if (executionStackIds.size() > 0) {
			currEntry->parent = executionStackIds.back();
			entry[executionStackIds.back()]->children.push_back(entryId);  // set child of the current parent
		}
		else
			currEntry->parent = -1;
		entry.push_back(currEntry);
	}
	else {
		entryId = it->second;
		currEntry = entry[entryId];
	}

	executionStackIds.push_back(entryId);

	currEntry->owntime += cvGetTickCount() - t1;

	currEntry->timestart = cvGetTickCount();

	mutex.Unlock();
}


void TimeProfileThread::Stop2(const char *file, const char *function, int line) {
map<string,int>::iterator it;
TimeProfileEntry *currEntry;
string str;
int entryId;
int64 endTime; 

	endTime = cvGetTickCount();

	mutex.Lock();
	str = executionStack[executionStack.size()-1];

	it = entryMap.find(str);
	if (it == entryMap.end()) 
		ReportError("Has to call Start first!");

	entryId = it->second;
	currEntry = entry[entryId];

	currEntry->total += endTime - currEntry->timestart;
	currEntry->count++;
	currEntry->ntimes++;
	currEntry->line1 = line;
	executionStack.pop_back();
	executionStackIds.pop_back();
	currEntry->owntime += cvGetTickCount() - endTime;

	mutex.Unlock();
}





int TimeProfileThread::PrintGraph(FILE *f, int idx0) {
TimeProfileEntry *currEntry;
queue<int> qEntry;
int v, i;
double totalTime1, totalTime2, totalTime3, totalTimeRoot, totalTime4; 
COLOR vcolor;
float percSum;
int idxExtraNode;

	// name the nodes
	totalTimeRoot = (double) entry[0]->total / (cvGetTickFrequency() * 1000000);
	for (i = 0; i < (int) entry.size(); i++) {
		currEntry = entry[i];
		if (i > 0 && currEntry->parent >= 0) {
			totalTime1 = (double) entry[currEntry->parent]->total / (cvGetTickFrequency() * 1000000);
			totalTime2 = (double) currEntry->total / (cvGetTickFrequency() * 1000000);
			totalTime3 = (totalTime2 / totalTime1) * 100.0f;
		}
		else {
			totalTime3 = 100.0f;
			totalTime2 = (double) currEntry->total / (cvGetTickFrequency() * 1000000);
			totalTime1 = totalTime2;
		}
		totalTime4 = 1000 * ((double) currEntry->total / (cvGetTickFrequency() * 1000000)) / (double) currEntry->ntimes;

		// percentage of the total time
		vcolor = GetColorHeatMap((float) (totalTime2 / totalTimeRoot), 0.0f, 1.0f);

		fprintf(f, "%d [style=filled fontcolor=\"#111111\" fillcolor=\"#%2.2X%2.2X%2.2X\"  shape=box label=\"[%s] %s():%d-%d [%dx] [%0.2f%%] [%0.2fs] [%0.5fms] [%0.2fx/sec]\"];\n", i + idx0, 
			(int) vcolor.r, (int) vcolor.g, (int) vcolor.b, currEntry->threadID.c_str(), currEntry->name.c_str(), currEntry->line0, 
			currEntry->line1, currEntry->count, totalTime3, totalTime2, totalTime4, (double) 1000.0 / totalTime4);
	}

	idxExtraNode = (int) entry.size();


	// draw the graph
	qEntry.push(0);

	while (qEntry.empty() == false) {
		
		v = qEntry.front();
		qEntry.pop();
		currEntry = entry[v];

		// draw the remaining time
		percSum = 0.0f;
		totalTime1 = 0;
		for (i = 0; i < (int) currEntry->children.size(); i++) {
			fprintf(f, "%d -> %d;\n", v + idx0, currEntry->children[i] + idx0);
			qEntry.push(currEntry->children[i]);
			percSum += (float) entry[currEntry->children[i]]->total / (float) currEntry->total;
			totalTime1 += (double) entry[currEntry->children[i]]->total / (cvGetTickFrequency() * 1000000);
		}
		totalTime2 = (double) currEntry->total / (cvGetTickFrequency() * 1000000);
		totalTime1 = totalTime2 - totalTime1;
		if (currEntry->children.size() > 0) {
			vcolor = GetColorHeatMap((float) (totalTime1 / totalTimeRoot), 0.0f, 1.0f);
			fprintf(f, "%d -> %d;\n", v + idx0, idxExtraNode + idx0);
			fprintf(f, "%d [style=filled fontcolor=\"#111111\" fillcolor=\"#%2.2X%2.2X%2.2X\" shape=box label=\"Unaccounted Time [%0.2f%%] [%0.2fs]\"];\n", idxExtraNode + idx0, (int) vcolor.r, (int) vcolor.g, (int) vcolor.b, 100.0f * (1.0f - percSum), totalTime1);
			idxExtraNode++;
		}
	}

	return idxExtraNode + idx0;
}



COLOR TimeProfileThread::GetColorHeatMap(float v, float vmin, float vmax) {
COLOR c = {1.0,1.0,1.0}; // white
float dv;

   if (v < vmin)
      v = vmin;
   if (v > vmax)
      v = vmax;
   dv = vmax - vmin;

   if (v < (vmin + 0.25f * dv)) {
      c.r = 0.0f;
      c.g = 4.0f * (v - vmin) / dv;
   } else if (v < (vmin + 0.5f * dv)) {
      c.r = 0;
      c.b = 1.0f + 4.0f * (vmin + 0.25f * dv - v) / dv;
   } else if (v < (vmin + 0.75f * dv)) {
      c.r = 4.0f * (v - vmin - 0.5f * dv) / dv;
      c.b = 0.0f;
   } else {
      c.g = 1.0f + 4.0f * (vmin + 0.75f * dv - v) / dv;
      c.b = 0.0f;
   }

   c.r = c.r * 255.0f;
   c.b = c.b * 255.0f;
   c.g = c.g * 255.0f;


   return(c);
}



void TimeProfile::Start2(const char *file, const char *function, int line) {
map<string, TimeProfileThread *>::iterator it;
TimeProfileThread *tpt;
string modID;

	modID = glb_SSFExecCtrl.GetInstanceIDCurrentThread();

	mutex.Lock();
	it = entries.find(modID);
	if (it == entries.end()) {
		tpt = new TimeProfileThread();
		entries.insert(pair<string, TimeProfileThread *>(modID, tpt));
	}
	else {
		tpt = it->second;
	}
	mutex.Unlock();

	tpt->Start2(file, function, line);
}



void TimeProfile::Stop2(const char *file, const char *function, int line) {
map<string, TimeProfileThread *>::iterator it;
TimeProfileThread *tpt;
string modID;

	
	modID = glb_SSFExecCtrl.GetInstanceIDCurrentThread();

	mutex.Lock();
	it = entries.find(modID);
	if (it == entries.end()) {
		tpt = NULL;
		ReportError("Error");
	}
	else {
		tpt = it->second;
	}
	mutex.Unlock();

	tpt->Stop2(file, function, line);
}



void TimeProfile::Printx() {
map<string, TimeProfileThread *>::iterator it;
FILE *f;
int idx = 0;
	

	/* first output */
	f = fopen("output.dot", "w");

	// headers
	fprintf(f, "digraph graphname {\n");
	fprintf(f, "rankdir=LR\n");
	fprintf(f, "ranksep=equally\n");

	for (it = entries.begin(); it != entries.end(); it++) {
		idx += it->second->PrintGraph(f, idx);
	}

	fprintf(f, "}\n");
	fclose(f);
}

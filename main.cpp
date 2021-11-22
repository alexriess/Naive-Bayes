#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "attribute.h"
#include "instance.h"
#include <iomanip>
#include <climits>

using namespace std;

double Prob(int cur_attribute, const vector<vector<vector<int>>>& big_c_table, vector<Attribute>& attribute_list, string cur_val, int class_num, int class_category){
  //gets fractions for specific classification case:
  //find attribute within attribute list -> Get_Nominal_Value and it's count -> outcome that matches up [j]
  //outlook -> sunny 2 -> 9
  double n, d;
  vector<string> cur_category_list = attribute_list[cur_attribute].Get_Category_List();

  //when I run into "sunny" as the current value
  //right now it's looping through an example twice (for however many examples there are)
  for(int j = 0; j < cur_category_list.size(); j++){
    if(cur_category_list[j] == cur_val){
      cout << cur_val << " ";
      //get count of "sunny" by looking it up in the table
      n = big_c_table[cur_attribute][j][class_num]+1;
      d = class_category + (attribute_list.size() - 1);      
      cout << n << "/" << d << endl;
    }
  } 
  return n / d;
}

// parses a line of data
void Parse_Data(string s, const vector<Attribute>&  attribute_list,
		vector<Instance>& example_list){
  Instance cur_instance;
  cur_instance.Set_Attributes(attribute_list);

  stringstream ss;
  ss.str(s);
  int cur_attribute = 0;
  string cur_val;
  while(ss >> cur_val){
    if(cur_val[cur_val.size()-1] == ',')
      cur_val = cur_val.substr(0, cur_val.size()-1); // trim off comma
    cur_instance.Add_Value(cur_attribute, cur_val);
    cur_attribute++;
  }
  if(cur_attribute != attribute_list.size()){
    cout << "ERROR: Wrong number of attributes on line: " << s << endl;
    exit(1);
  }
  example_list.push_back(cur_instance);
  
}

// gets a new attribute from an @attribute line
void New_Attribute(string s, vector<Attribute>& attribute_list){
  Attribute cur_attribute;
  stringstream ss;
  ss.str(s);
  string junk;
  ss >> junk; // remove "@attribute"
  string name;
  ss >> name;
  cur_attribute.Set_Name(name);
  string next; // either "{" or "numeric"
  ss >> next;
  if(next == "numeric")
    cur_attribute.Set_Numeric(true);
  else{
    cur_attribute.Set_Numeric(false);
    string temp;
    while(temp != "}"){
      ss >> temp;
      if(temp != "}"){
	if(temp[temp.size()-1] == ',')
	  temp = temp.substr(0, temp.size()-1); // trim off comma
	cur_attribute.Add_Category(temp);
      }
    }
  }
  attribute_list.push_back(cur_attribute);
}
     
void Print_Data(vector<Attribute>& attribute_list, vector<Instance>& examples){
  char choice = 'y';
  while(choice == 'y'){
    cout << "We have " << examples.size() << " examples. Which would you like? ";
    int cur_example;
    cin >> cur_example;

    cout << "Type 0 to see the all attributes" << endl;
    for(int i = 0; i < attribute_list.size(); i++){
      cout << "Type " << i+1 << " to see value of attribute " << attribute_list[i].Get_Name() << endl;
    }
    int which_attribute;
    cin >> which_attribute;
    for(int i = 0; i < attribute_list.size(); i++){
      if(which_attribute == 0 || which_attribute == i+1){
	if(attribute_list[i].Numeric())
	  cout << attribute_list[i].Get_Name() << ": "
	       << examples[cur_example].Get_Numeric_Value(i) << endl;
	else
	  cout << attribute_list[i].Get_Name() << ": "
	       << examples[cur_example].Get_Nominal_Value(i) << endl;
      }
    }
    cout << "Another? (y/n) ";
    cin >> choice;
  }
}



int Get_Number_Of_Category(const vector<Attribute>& attribute_list, int cur_attribute, string my_category){
	
	//sunny, overcast, rainy
	vector<string> names = attribute_list[cur_attribute].Get_Category_List();
	
	for(int i = 0; i < names.size(); i++){
		//	0 == 0
		if(my_category == names[i]){
			return i;
		}
			
	}
	return -1;
}
//the tester for attribute: Outlook
void Make_Count_Table(const vector<Attribute>& attribute_list, const vector<Instance>& examples, int cur_attribute, vector<vector<int> >& Table){
	
	//take care of the rows
	Table.resize(attribute_list[cur_attribute].Num_Categories());
	
	//go into the rows to fill the columns 
	for(int i = 0; i < Table.size(); i++){
		//finding "play" as classification. This is the column size
		Table[i].resize(attribute_list[attribute_list.size()-1].Num_Categories());
		
	}
	

	for(int j = 0; j < examples.size(); j++){
		//go inside table and find current example and find the "play" attribute categories			// 0 or 1 for yes or no
    //Table[a][b]++
    //Table[row...going into examples with atttributes][column...classification categories]
    Table[Get_Number_Of_Category(attribute_list, cur_attribute, examples[j].Get_Nominal_Value(cur_attribute))][Get_Number_Of_Category(attribute_list, attribute_list.size() - 1, examples[j].Get_Nominal_Value(attribute_list.size()-1))]++; 
			
	}
}  




void Print_Count_Table(vector<vector<int> >& Table, vector<Attribute> attribute_list, int att_num){
  //attempt of having pretty spacing and alignment
  cout << setw(20) << " ";
  for(int b = 0; b < Table[0].size(); b++){
    cout << setw(20) << attribute_list[attribute_list.size() - 1].Get_Category(b);
  }
  cout << endl;
  for( int i = 0; i < Table.size(); i++){
    cout << left << setw(20)<< attribute_list[att_num].Get_Category(i);
    for(int j = 0; j < Table[0].size(); j++){
      cout << setw(20)<< Table[i][j] ;
    }
  cout << endl << endl;
  }
}


void Make_1R_Rule(const vector<Attribute>& attribute_list,const vector<vector<int> >& count_table, int cur_attribute, vector<int>& cur_rule){

  for(int i=0; i< count_table.size();i++){
    int max_num = 0;
    for(int j = 0; j < count_table[i].size(); j++){
        //maximum value out of those
        if(count_table[i][max_num] < count_table[i][j]){
          max_num = j;
          
        }
    }
    //pushing back the maximum number of values of each attribute to the cur_rule vector
    cur_rule.push_back(max_num);
    
  }


}


void Print_1R_Rule(vector<int>& cur_rule, vector<Attribute> attribute_list, int attribute_num){
  cout << "The rule for attribute: " << attribute_list[attribute_num].Get_Name() << endl;

  for(int i = 0; i < attribute_list[attribute_num].Get_Category_List().size(); i++){
    //getting the current category 
    //getting category out based on the rule
    cout << "if " << attribute_list[attribute_num].Get_Category(i) << " then " << attribute_list[attribute_list.size()-1].Get_Category(cur_rule[i]) << endl;
  
  }
 
}

double Calc_Error_Rates(vector<int>& cur_rule, int cur_attribute, vector<Instance>& examples, vector<Attribute>& attribute_list){
   
  double num_right = 0;
  
  //taking the rule(and its attribute number)
  //and all the examples
  for(int i = 0; i < examples.size(); i++ ){
    for(int j = 0; j < cur_rule.size(); j++){
        //counting how many examples were right
        if(examples[i].Get_Nominal_Value(cur_attribute) == attribute_list[cur_attribute].Get_Category(j) && examples[i].Get_Nominal_Value(attribute_list.size() - 1) == attribute_list[attribute_list.size() - 1].Get_Category(cur_rule[j])){
        num_right++;
        
      }
    }
  }
  // determine how many examples were wrong
  return (examples.size() - num_right) / examples.size();
}


//_______________________________________________________________________________
int main(){
  string filename;
  cout << "Enter the filename: ";
  cin >> filename;

  ifstream fin;
  fin.open(filename.data());
  while(!fin){
    cout << "File not found" << endl;
    cout << "Enter the filename: ";
    cin >> filename;
    fin.clear();
    fin.open(filename.data());
  }
  vector<Attribute> attribute_list;
  vector<Instance> examples;

  // file is open
  bool data_mode = false;
  string s;
  int num_read = 0;
  while(getline(fin, s)){
    if(s[s.size()-1] == '\r') // grrr
      s = s.substr(0,s.size()-1);
    
    if(s.size() > 0 && s[0] != '%'){ // ignore comments
      if(data_mode){ // it's a line of data
	Parse_Data(s, attribute_list, examples);
      }
      else{
	// then it had better start with an '@'
	if(s[0] != '@'){
	  cout << "ERROR: ILLEGAL LINE: " << s << endl;
	  exit(1);
	}
	// is it @attribute?
	else if(s.substr(0,10) == "@attribute")
	  New_Attribute(s, attribute_list);
	//is it @data?
	else if(s.substr(0,5) == "@data")
	  data_mode = true;
	else{
	  cout << "ERROR: ILLEGAL LINE: " << s << endl;
	  exit(1);
	}
      }
    }
    num_read++;
   // cout << num_read << endl;
  }
// Now that the data is read, what should we do?
 Print_Data(attribute_list, examples);
//----------------------------------------------------------------------------------------------------------------------------------------------------
 

  
	int cur_attribute;
  int num;

  double error_rate;
  double best_error = 1;
  int best_rule;
  vector<vector<int>> rules;
  vector<int> cur_rule;
  vector<vector<vector<int>>> Big_Count_Table;


  for(int i = 0; i < attribute_list.size() - 1; i++){
    
    vector<vector<int>> Table;
    Make_Count_Table(attribute_list, examples, i, Table);

	  Print_Count_Table(Table, attribute_list, i);

    vector<int> cur_rule;
    
    Make_1R_Rule(attribute_list, Table, i, cur_rule);

    Print_1R_Rule(cur_rule, attribute_list, i);

    error_rate = Calc_Error_Rates(cur_rule, i, examples, attribute_list);

    cout << "With error rate: " << error_rate << endl;
     
    
    if(error_rate < best_error){
       best_error = error_rate;
       best_rule = i;
    }
    rules.push_back(cur_rule);
    
    //taking in attribute values into Table
    //meaning [cur attribute][][]
    Big_Count_Table.push_back(Table);
    cout << endl;
    cout << endl;
    
  }
  
  
  //go through attribute list error rates now
  //find the best one 
  cout << "The best 1R attribute is " << best_error  << " which is " << attribute_list[best_rule].Get_Name() << endl;

  
  //FINAL PART:
  //the testing 

  string test_filename;
  //copy and paste file syntax
  cout << "Enter the test data file: ";
  cin >> test_filename;
  ifstream test_fin;
  test_fin.open(test_filename.data());
  while(!test_fin){
    cout << "File not found" << endl;
    cout << "Enter the filename: ";
    cin >> test_filename;
    test_fin.clear();
    test_fin.open(test_filename.data());
  }
  
  vector<Instance> test_examples;

  // file is open
  bool test_data_mode = true;
  string test_s;
  int test_num_read = 0;
  while(getline(test_fin, test_s)){
    if(test_s[test_s.size()-1] == '\r') // grrr
      test_s = test_s.substr(0, test_s.size()-1);
    if(test_s.size() > 0 && test_s[0] != '%'){ // ignore comments
      if(test_data_mode == true){ // it's a line of data
	      Parse_Data(test_s, attribute_list, test_examples);
        
      }
    
    }
    test_num_read++;
    cout << test_num_read << endl;
  
  }
  
  Print_Data(attribute_list, test_examples);

  double test_error_rate = Calc_Error_Rates(rules[best_rule], best_rule, test_examples, attribute_list);
  cout << "The error rate on the test data is: " << test_error_rate << endl;


//PREPROCESSING
//Count how many times each outcome of classification attribute happens
 // Big_Count_Table[attribute][row][next column]
 // Big_Count_Table[attribute][attribute[category][classification values]
  vector<double> outcomes;
  int sum = 0;
  // builds columns                   
  for(int i = 0; i < Big_Count_Table[0][0].size(); i++){
      cout << "c: " << i << endl;
      // builds rows 
    for(int j = 0; j < Big_Count_Table[0].size(); j++){
      cout << "r: " << j << endl;
        sum += Big_Count_Table[0][j][i];
        
        
    }
    cout << sum << endl;
    outcomes.push_back(sum);
    sum = 0;
  }
  //outcomes[i] spits out 9 and 5
  //Observed likelihoods of each attribute's value

  //temp for likelihood being stored
  double likelihood = 1;
  vector<vector<double>> likelihoods;
  likelihoods.resize(test_examples.size());
  //as we go through each test example
  //starting with yes
  for(int i = 0; i < test_examples.size(); i++){
      cout << "example: "<< i + 1 << endl;
      //starting with 9
      for(int j = 0; j < outcomes.size(); j++){
        //compute observed likelihood of each attribute's value
        for(int k = 0; k < attribute_list.size()-1; k++){
          //once we've gotten the fractions
          double probabilities = Prob(k, Big_Count_Table, attribute_list, test_examples[i].Get_Nominal_Value(k), j, outcomes[j]);//j is our current classif category
          // multiply
          //1 = 1 * 2/9
          //2/9 = 2/9 * 3/9
          likelihood *= probabilities;  
        }
        //double likelihood = likelihood * (outcomes[j] / final classification
        likelihood *= (outcomes[j] / examples.size());
        cout << "likelihood is " << likelihood << endl;

        //store it for later when we use all the likelihoods
        likelihoods[i].push_back(likelihood);
        //then reset the double
        likelihood = 1;
       
      }
      //computing probabilities of each outcome.
      //likelihood = likelihood / likelihoods
		  double n = 0, d = 0;
		  for (int j = 0; j < outcomes.size(); j++) {
			  for (int k = 0; k < likelihoods[i].size(); k++) {
          //getting the denominator 
				  d += likelihoods[i][k];	
			  }
        //getting the numerator 
			  n = likelihoods[i][j];	
        
        //calculating probabiltiy to get proper percentage
			  double probability = 100 * n / d;	
			  cout << attribute_list[attribute_list.size() - 1].Get_Category(j) << "% = " << probability << endl;
        //reseting num and den
			  n = 0, d = 0;	
		  }
		cout << endl;
	}

  //
  ///Homework
  //

  //test what the classifier using two different ideas
  //pick the higher probability between yes and no for the instances

  //a points system for both 
  vector<double> scores_id1;
	vector<double> scores_id2;
  
	scores_id1.resize(likelihoods[0].size(), 0);
	scores_id2.resize(likelihoods[0].size(), 0);
  //for the likelihoods 
	for (int i = 0; i < likelihoods.size(); i++) {

		int index = 0;
    //"highest" outcome 
    double highest = INT_MIN;
    //for each likelihood
		for (int j = 0; j < likelihoods[i].size(); j++) {
      //add the probability to the corresponding category
			scores_id2[j] += likelihoods[i][j];	
			if (likelihoods[i][j] > highest) {
        //the max probability
				highest = likelihoods[i][j];
        //store the max probability category's index	
				index = j;
			}
		}
    //max probability for the instance gets a point
   // cout << "point" << scores_id1[index] << endl;
		scores_id1[index]++;	
    
	}
  
  //display the scores
  cout << "-EVALUATION-" << endl;
  //idea 1
  cout << "idea 1 scores:" << endl;
  for(int i = 0; i < scores_id1.size(); i++){
    
    cout << attribute_list[attribute_list.size()-1].Get_Category(i) << " = " << scores_id1[i] << endl;

  }

  //idea 2
  cout << "idea 2 scores:" << endl;
  for(int j = 0; j < scores_id2.size(); j++){
    
    cout << attribute_list[attribute_list.size()-1].Get_Category(j) << " = " << scores_id2[j] << endl; 

  }

  return 0;
}
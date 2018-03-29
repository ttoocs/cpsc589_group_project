//This assumes ground is at z = 0
void trace_lightning_recursion(vec3 init_point, vec3 init_direction, vector<vec3> *storage, vec3 original)
{
	vec3 rand_segment = vec3(0,0,0);
	vec3 current_point = vec3(0,0,0);
	double rand_angle = 0.0f;
	double angle;
	storage->push_back(init_point);
	
	while(current_point.z > 0 && /*Random end check*/)
	{
		//START: Random angle, random vector
		
		//END: Random angle, random vector
		
		//This
		if(/*Random check to branch*/)
		{
			trace_lightning(current_point, /*new direction*/, storage);
		}
 	}
}

//This assumes ground is at z = 0
void trace_lightning(vec3 init_point, vec3 init_direction, vector<vec3> *storage)
{
	vec3 rand_segment = vec3(0,0,0);
	vec3 current_point = init_point;
	double rand_angle = 0.0f;
	double angle;
	
	storage->push_back(init_point);
	while(current_point.z > 0)
	{
		
		/* This is for branching
		if(Random check to branch)
		{
			//START: Get new direction for the branch
			
			//END: Get new direction for the branch
			trace_lightning_recursion(current_point, new direction, storage, init_direction);
			storage->push_back(current_point);
		}
		*/
		//START: Random angles, get rand segment
		
		//END: Random angles, get rand segment
		//Here, we should have the new segment direction and length
		
		current_point += rand_segment;
		storage->push_back(current_point);
		storage->push_back(current_point);
 	}
}

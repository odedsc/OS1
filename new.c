// Increase prediction by one (for instance, WT -> ST)
prediction increase_prediction(prediction curr)
{
	if (curr < ST)
	{
		return (curr+1);
	}
	else return curr;
}

// Decrease prediction by one (for instance, WT -> WNT)
prediction decrease_prediction(prediction curr)
{
	if (curr > SNT)
	{
		return (curr-1);
	}
	else return curr;
}

unsigned int update_history(unsigned int curr_hist, bool taken) {
	curr_hist <<= 1 ;
	curr_hist = taken? (curr_hist + 1) : (curr_hist);
	return (curr_hist % (1 << history_Size));
}



void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst)
{
	// Update the SIM_stats:
	curr_stats.br_num++ ;
	bool was_correct = false;
	if (taken)
	{
		if (targetPc == pred_dst)
		{
			was_correct = true;
		}
	}
	else
	{
		if ((pc+4) == pred_dst)
		{
			was_correct = true;
		}
	}
	if (was_correct == false)
	{
		curr_stats.flush_num++ ;
	}

	//Update the BTB:
	int BTB_index = ((pc / 4) % btb_Size);
	BTB_line* curr_line = &(BTB[BTB_index]);

	// check the need to replace this BTB entry,
	// which means: if the entry is empty or the tag isn't matching:
	bool same_Tag = false;
	int pc_tag = ((pc / 4) % (1 << tag_Size));
	if (tag == pc_tag)
		{
		same_Tag = true;
		}
	if ((curr_line->tag == -1) || (same_Tag==false)
		{

		//Add BTB entry:
		curr_line->tag = ((pc / 4) % (1 << tag_Size));
		curr_line->target = targetPc;

		//update the relevant state machine:
		//get the current history value:
		unsigned int hist_index;
		if (is_global_hist)
		{



			/////// need to check what nitai has done with the function calculate_hist_val






			hist_index = calculate_hist_index(pc, BTB_index); //calculate the history value (not updated yet)
		}
		else
		{
			if (is_share && isGlobTable)
			{
				unsigned int pc_share_val = (pc / 4) % (1 << history_Size) ;
				hist_index = (0 ^ pc_share_val) ; // XOR in c
			}
			else
			{
				hist_index = 0;
			}
		}
		if (isGlobTable)
		{
			//update the global state machine:
			Global_States[hist_index] = (taken? increase_prediction(Global_States[hist_index]) : decrease_prediction(Global_States[hist_index]))
		}
		else
		{
			//Restart the relevant local state machine:
			for (int i=0; i < States_max_size; i++)
			{
				Local_States[BTB_index][i] = WNT;
			}

			//Update the specific state in the relevant state machine (should be 0)
			Local_States[BTB_index][hist_index] = (taken? increase_prediction(Local_States[BTB_index][hist_index]) : decrease_prediction(Local_States[BTB_index][hist_index]))
		}

		//update the relevant history value:
		if (isGlobHist)
		{
			/////// need to check what nitai has done with the function calculate_hist_val
			global_history = update_history (global_history, taken);
		}
		else
		{
			curr_line->LocalHistory = update_history (0, taken);
		}

	}
	else
	{ //BTB entry already contains this branch:

		//Update this BTB entry:
		curr_line->target = targetPc;

		//get the current history value:
		unsigned int hist_index = calculate_hist_index(pc, BTB_index); //calculate the history value (not updated yet)

		//update the relevant state machine:
		if (isGlobTable)
		{
			//update the global state machine:
			Global_States[hist_index] = (taken? increase_prediction(Global_States[hist_index]) : decrease_prediction(Global_States[hist_index]));
		}
		else
		{
			//Update the specific state in the relevant state machine:
			Local_States[BTB_index][hist_index] =  = (taken? increase_prediction(Local_States[BTB_index][hist_index]) : decrease_prediction(Local_States[BTB_index][hist_index]));

		}

		//update the relevant history value:
		if (isGlobHist)
		{
			//check nitai

			global_history = update_history(global_history, taken);
		}
		else
		{
			curr_line->local_history = update_history(curr_line->local_history, taken);
		}

	}
	return;
}

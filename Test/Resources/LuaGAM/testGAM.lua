local function process_commands(time_ms)
	if Hold_cmd == 1 then -- process hold command
		Ramp_done = 0
		State = 0
	elseif Ramp_cmd == 1 then -- process ramp up command
		Last_time = time_ms
		Ramp_done = 0
		State = Ramp_cmd
	elseif Ramp_cmd == 2 then -- process ramp down command
		Last_time = time_ms
		Ramp_done = 0
		State = Ramp_cmd
	end
end

local function sweep(time_ms)
	local dt = time_ms - Last_time -- compute delta time
	local dI = Sweep_rate / 60000.0 -- min to ms
	if State == 2 then             -- when ramping down (meaning going to 0)
		Current = Current - dI * dt  -- decrease current
		if Current <= 0.0 then       -- check if FlatZero is reached
			Current = 0.0
			Ramp_done = state
			State = 4
		end
	elseif State == 1 then         -- when ramping up (meaning go to the set point)
		if Current < Current_sp then -- when set point is greater then current value
			Current = Current + dI * dt -- increase cuurent
			if Current >= Current_sp then -- check if set point is reached and turn flag FlatTop
				Current = Current_sp
				Ramp_done = State
				State = 3
			end
		else                         -- when set poin is less then current value
			Current = Current - dI * dt -- decrease current
			if Current <= Current_sp then -- check if set poin is reached and turn flag FlatTop
				Current = Current_sp
				Ramp_done = State
				State = 3
			end
		end
	end
end

function GAM()
	local time_ms = marte.mtime() -- time in ms
	process_commands(time_ms)
	if State == 1 or State == 2 then
		sweep(time_ms)
	end
	marte.log("State: " .. tostring(State))
	Last_time = time_ms
end

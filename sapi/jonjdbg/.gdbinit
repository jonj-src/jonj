define ____jonjdbg_globals
	if basic_functions_module.zts
		if !$tsrm_ls
			set $tsrm_ls = ts_resource_ex(0, 0)
		end
		set $jonjdbg = ((zend_jonjdbg_globals*) (*((void ***) $tsrm_ls))[jonjdbg_globals_id-1])
	else
		set $jonjdbg = jonjdbg_globals
	end
end

delete from auth_token where auth_info_id in
	(select auth_info_id from auth_token where auth_info_id in
		(select auth_info_id from auth_token group by auth_info_id having count(id) > 40)
	order by expires
	limit (select count(*) from auth_token where auth_info_id in
		(select auth_info_id from auth_token group by auth_info_id having count(id) > 40)) / 2);

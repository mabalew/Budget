create table expenses(
		id integer primary key autoincrement,
		exp_date datetime default CURRENT_TIMESTAMP,
		amount double default 1.0,
		product_id integer not null,
		shop_id integer default 0,
		price double,
		foreign key(shop_id) references shops(id),
		foreign key(product_id) references products(id)
	);

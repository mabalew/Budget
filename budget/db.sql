CREATE TABLE categories(id integer primary key autoincrement, category_name varchar2(100));
CREATE TABLE products(id integer primary key autoincrement, product_name varchar2(100), category_id int, foreign key(category_id) references categories(id));
CREATE TABLE shops(id integer primary key autoincrement, shop_name varchar2(100) unique);
CREATE TABLE users(id integer primary key autoincrement, login varchar2(50), password varchar2(32));
CREATE TABLE expenses(id integer primary key autoincrement, exp_date datetime default CURRENT_TIMESTAMP, amount double default 1.0, product_id integer not null, shop_id integer default 0, price double, foreign key(shop_id) references shops(id), foreign key(product_id) references products(id));
CREATE VIEW v_expenses as
select e.id [exp id],
	e.exp_date,
	c.id [category_id],
	c.category_name,
	p.id [product_id],
	p.product_name,
	e.amount,
	e.price,
	(1 / e.amount) * e.price [unit_price],
	s.id [shop_id],
	s.shop_name
from
	expenses e, categories c, products p, shops s
where
	p.category_id = c.id and
	e.product_id = p.id and
	e.shop_id = s.id
group by e.id;

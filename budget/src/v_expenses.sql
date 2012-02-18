create view v_expenses as
select
	e.id [exp id],
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


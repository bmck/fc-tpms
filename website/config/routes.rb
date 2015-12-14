# $Id$
# $(c)$

Rails.application.routes.draw do
  # The priority is based upon order of creation: first created -> highest priority.
  # See how all your routes lay out with "rake routes".

  devise_for :user, controllers: { sessions: "user/sessions", registrations: "user/registrations" }

  # You can have the root of your site routed with "root"
  # root 'welcome#index'

  root to: 'main#index'

  get '/errors/internal_server_error' => 'errors#internal_server_error'
  get '/errors/not_found' => 'errors#not_found'
  get '/errors/unprocessable_entity' => 'errors#unprocessable_entity'

  get '/home' => 'main#home'

  resources :companies, :users, :sensors, :receivers, :tires, :tire_types, :tire_samples, :trailers, :trucks, :storages do
    member do
      get 'view_audit'
    end
  end

  resources :tire_sample_report, only: [:new, :create]

  namespace :api do
    namespace :v1 do
      resources :trucks, :tires, only: [:index], via: :get
      resources :tire_samples, only: [:index, :create], via: :get
    end
  end
end
